/* Copyright (C) 2023 Gramine contributors
 * SPDX-License-Identifier: BSD-3-Clause */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

/*
 * 
 */

#define IO_DEV      "/dev/pseudo-io-device"

#define IOCTL_OUTPUT_U64DATA    0xBABF

#define OUTPUT_DATA             0x4141414141414141


int main(void) {    
    int dev_fd; 
    dev_fd = open(IO_DEV, O_RDWR);
    
    if (dev_fd < 0) {
        perror("open");
        return -1;
    }

    ioctl(dev_fd, IOCTL_OUTPUT_U64DATA, OUTPUT_DATA);

    printf("====== Hello, world. Demo sandboxed program start ======.\n");
    printf("For test/demo purpose, this program does not require any input.\n");
    printf("For test, sandboxes are allowed to print stats to this console.\n");
    printf("This program will send the output data as 'AAAAAAAA' (0x4141...).\n");
    
    printf("====== Hello, world. Demo sandboxed program done ======.\n");
    printf("The output is sent to the monitor.\n");
    printf("Please execute the following command to see the output:\n");
    printf("sudo cat /sys/kernel/debug/encos-output-emulate/out\n");
    printf("=======================================================\n");
    printf("You are expected to see the output data 'AAAAAAAA' (0x4141...).\n");
    return 0;
}
