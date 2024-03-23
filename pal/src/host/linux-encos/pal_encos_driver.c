#include "pal.h"
#include "syscall.h"
#include "pal_encos_driver.h"

/* MMAP-related headers */
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
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
