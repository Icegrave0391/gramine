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

#define OUTPUT_DATA             0xCAFEBABE


int main(void) {    
    int dev_fd; 
    dev_fd = open(IO_DEV, O_RDWR);
    
    if (dev_fd < 0) {
        perror("open");
        return -1;
    } else {
        printf("Device opened successfully, fd=%d\n", dev_fd);
    }

    ioctl(dev_fd, IOCTL_OUTPUT_U64DATA, OUTPUT_DATA);

    printf("Hello, world\n");
    return 0;
}
