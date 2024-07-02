#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define TIMEOUT_SECONDS 5

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int signaled;
} Event;

volatile bool running = true; // global flag to stop all threads

void EventInit(Event* event) {
    pthread_mutex_init(&event->lock, NULL);
    pthread_cond_init(&event->cond, NULL);
    event->signaled = 0;
}

void EventDestroy(Event* event) {
    pthread_mutex_destroy(&event->lock);
    pthread_cond_destroy(&event->cond);
}

void EventWait(Event* event) {
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += TIMEOUT_SECONDS;

    pthread_mutex_lock(&event->lock);
    while (event->signaled == 0 && running) {
        if (pthread_cond_timedwait(&event->cond, &event->lock, &timeout) == ETIMEDOUT) {
            printf("Consumer: wait timeout\n");
            pthread_mutex_unlock(&event->lock);
            return;
        }
    }
    if (running) {
        event->signaled = 0; // Reset event for auto-clear behavior
        printf("Consumer: event received\n");
    }
    pthread_mutex_unlock(&event->lock);
}

void EventSet(Event* event) {
    pthread_mutex_lock(&event->lock);
    event->signaled = 1;
    pthread_cond_signal(&event->cond); // Wake up one waiting thread
    pthread_mutex_unlock(&event->lock);
}

void* Producer(void* arg) {
    Event* event = (Event*)arg;
    while (running) {
        usleep(1000000); // Simulate work by sleeping for 1 second
        printf("Producer: setting event\n");
        EventSet(event);
    }
    return NULL;
}

void* Consumer(void* arg) {
    Event* event = (Event*)arg;
    while (running) {
        printf("Consumer: waiting for event\n");
        EventWait(event);
    }
    return NULL;
}

int main() {
    Event event;
    EventInit(&event);

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, Producer, &event);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, Consumer, &event);
    }

    /* let's control the execution time of all threads */
    sleep(5); // 5s

    running = false; // notify all threads to exit

    /* wait for all threads to exit */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    EventDestroy(&event);

    return 0;
}
