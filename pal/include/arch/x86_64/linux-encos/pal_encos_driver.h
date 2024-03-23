#ifndef __PAL_ENCOS_DRIVER_H__
#define __PAL_ENCOS_DRIVER_H__

#include <pal.h>

#include <stddef.h>
#include <stdint.h>

#define ENCOS_DEV       "/dev/encos-dev"

extern int g_encos_fd;

int open_encos_driver(void);

static inline int encos_fd(void)
{
    if (g_encos_fd < 0) {
        return open_encos_driver();
    } 
    else {
        log_always("encos_dev=%s g_encos_fd=%d.\n", ENCOS_DEV, g_encos_fd);
        return g_encos_fd;
    }
}
#endif