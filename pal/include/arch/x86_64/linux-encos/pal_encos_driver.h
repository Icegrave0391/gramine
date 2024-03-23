#ifndef __PAL_ENCOS_DRIVER_H__
#define __PAL_ENCOS_DRIVER_H__

#include <pal.h>

#include <stddef.h>
#include <stdint.h>

#define ENCOS_DEV       "/dev/encos-dev"

extern int g_encos_fd;

int open_encos_driver(void);

#define ENCOS_FD \
    (g_encos_fd < 0 ? open_encos_driver() : g_encos_fd)

#endif