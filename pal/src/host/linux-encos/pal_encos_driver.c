#include "pal.h"
#include "syscall.h"
#include "pal_encos_driver.h"

/* MMAP-related headers */
#include <asm/fcntl.h>
#include <asm/mman.h>
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h>
// #include <fcntl.h>

/* IOCTL-related headers */
#include <linux/ioctl.h>
#include <sys/ioctl.h>


/* global encos_fd */
int g_encos_fd = -1;

int open_encos_driver(void) 
{
    /* open and initialize */
    if (g_encos_fd < 0) {
        g_encos_fd = DO_SYSCALL(open, ENCOS_DEV, O_RDONLY);
    }

    if (g_encos_fd < 0) {
        log_error("Error: could not open ENCOS driver (%s)\n", ENCOS_DEV);
        return -1;
    }

#ifdef ENCOS_DEBUG
    log_always("Opened ENCOS driver (%s) to g_encos_fd=%d.\n",
                ENCOS_DEV, g_encos_fd);
#endif
    /* Return that everything works */
    //pal_printf("[*] Opened device driver successfully.\n");
    return g_encos_fd;
}

/* test function */
int test_mmap(void) 
{
    char *addr;
    int fd;
    // forcibly open
    fd = DO_SYSCALL(open, ENCOS_DEV, O_RDONLY);
    if (fd < 0) {
        log_error("Error: could not open ENCOS driver (%s)\n", ENCOS_DEV);
        return -1;
    } else {
        log_always("test force-open ENCOS driver (%s) to fd=%d.\n", ENCOS_DEV, fd);
    }

    // fake mmap
    addr = (char *)DO_SYSCALL(mmap, NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
    if (IS_PTR_ERR(addr)) {
        log_error("Error: could not mmap\n");
        return -1;
    }
    *addr = 'a';
    *(addr+1) = '\0';
    log_always("addr[0x%lx]: %s\n", (unsigned long)addr, addr);

    // unmap 
    DO_SYSCALL(munmap, addr, 4096);
    // close
    DO_SYSCALL(close, fd);
    
    return 0;
}