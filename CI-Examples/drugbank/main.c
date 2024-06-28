/*
 * A unit test and example of how to use the simple C hashmap
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>   /* time() */

#include "hashmap.h"

#define KEY_MAX_LENGTH (205)
#define KEY_PREFIX ("somekey")
// #define KEY_COUNT (1024*1024)
#define KEY_COUNT 2086719

#define FILENAME "database.txt"
#define LINE_LENGTH 200

typedef struct data_struct_s
{
    char key_string[KEY_MAX_LENGTH];
    int number;
} data_struct_t;


void read_lines_from_memory(char *mapped_data, size_t file_size) {
    size_t offset = 0;
    while (offset < file_size) {
        // Print the line
        fwrite(mapped_data + offset, 1, LINE_LENGTH, stdout);
        printf("\n");
        // Move to the next line
        offset += LINE_LENGTH + 1; // +1 for the newline character
    }
}

int main(char* argv, int argc)
{
    int index;
    int error;
    map_t mymap;
    char key_string[KEY_MAX_LENGTH];
    data_struct_t* value;
    
    int fd; // database
    struct stat file_info;
    char *mapped_data;

    clock_t t0, t1, t2, t3;
    // Open the file
    fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Get the file size
    if (fstat(fd, &file_info) == -1) {
        perror("Error getting file size");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Memory-map the file
    t0 = clock();
    mapped_data = mmap(NULL, file_info.st_size, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);
    if (mapped_data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        exit(EXIT_FAILURE);
    }    
    // Close the file descriptor; the file remains mapped in memory
    close(fd);

    mymap = hashmap_new();

    /* First, populate the hash map with ascending values */
    for (index=0; index<KEY_COUNT; index+=1)
    {
        /* Store the key string along side the numerical value so we can free it later */
        value = malloc(sizeof(data_struct_t));


        // Calculate the offset for the line in the memory-mapped file
        size_t offset = index * (LINE_LENGTH + 1); // +1 for the newline character
        if (offset >= file_info.st_size) {
            fprintf(stderr, "Error: Offset out of bounds\n");
            free(value);
            break;
        }
        snprintf(value->key_string, KEY_MAX_LENGTH, "%s%d%.*s", KEY_PREFIX, index, LINE_LENGTH, mapped_data + offset);
        // printf("Index=%d, key_string=%s\n", index, value->key_string);
        value->number = index;

        error = hashmap_put(mymap, value->key_string, value);
        assert(error == MAP_OK);

        // snprintf(value->key_string, KEY_MAX_LENGTH, "%s%d", KEY_PREFIX, index);
        // value->number = index;

        // error = hashmap_put(mymap, value->key_string, value);
        // assert(error==MAP_OK);
    }
    t1 = clock();

    printf("Time to insert %d elements: %f\n", KEY_COUNT, (double)(t1 - t0) / CLOCKS_PER_SEC);
    t2 = clock();
    /* Now, check all of the expected values are there */
    for (index=0; index<KEY_COUNT; index+=1)
    {
        size_t offset = index * (LINE_LENGTH + 1); // +1 for the newline character
        if (offset >= file_info.st_size) {
            fprintf(stderr, "Error: Offset out of bounds\n");
            break;
        }
        snprintf(key_string, KEY_MAX_LENGTH, "%s%d%.*s", KEY_PREFIX, index, LINE_LENGTH, mapped_data + offset);
        error = hashmap_get(mymap, key_string, (void**)(&value));
        
        /* Make sure the value was both found and the correct number */
        assert(error==MAP_OK);
        assert(value->number==index);
    }
    
    t3 = clock();

    printf("Time to fetch %d elements: %f\n", KEY_COUNT, (double)(t3 - t2) / CLOCKS_PER_SEC);

    // /* Make sure that a value that wasn't in the map can't be found */
    // snprintf(key_string, KEY_MAX_LENGTH, "%s%d", KEY_PREFIX, KEY_COUNT);

    // error = hashmap_get(mymap, key_string, (void**)(&value));
        
    // /* Make sure the value was not found */
    // assert(error==MAP_MISSING);

    // /* Free all of the values we allocated and remove them from the map */
    // for (index=0; index<KEY_COUNT; index+=1)
    // {
    //     size_t offset = index * (LINE_LENGTH + 1); // +1 for the newline character
    //     if (offset >= file_info.st_size) {
    //         fprintf(stderr, "Error: Offset out of bounds\n");
    //         free(value);
    //         break;
    //     }
    //     snprintf(key_string, KEY_MAX_LENGTH, "%s%d%.*s", KEY_PREFIX, index, LINE_LENGTH, mapped_data + offset);
    //     // snprintf(key_string, KEY_MAX_LENGTH, "%s%d", KEY_PREFIX, index);

    //     error = hashmap_get(mymap, key_string, (void**)(&value));
    //     assert(error==MAP_OK);

    //     error = hashmap_remove(mymap, key_string);
    //     assert(error==MAP_OK);

    //     free(value);        
    // }
    
    // /* Now, destroy the map */
    // hashmap_free(mymap);
    // free(mapped_data);

    return 1;
}