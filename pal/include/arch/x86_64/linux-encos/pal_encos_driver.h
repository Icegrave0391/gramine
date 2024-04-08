#ifndef __PAL_ENCOS_DRIVER_H__
#define __PAL_ENCOS_DRIVER_H__

#include <pal.h>

#include <stddef.h>
#include <stdint.h>
#include <common.h>

#define ENCOS_DEV       "/dev/encos-dev"

/* todo: use an untrusted memory as the futex now */
extern int g_assign_futex;

extern int g_encos_fd;

int open_encos_driver(void);

static inline int encos_fd(void)
{
    // return -1;   // debug purpose
    if (g_encos_fd < 0) {
        return open_encos_driver();
    } 
    else {
        log_always("encos_dev=%s g_encos_fd=%d.\n", ENCOS_DEV, g_encos_fd);
        return g_encos_fd;
    }
}

int encos_init_enclave(void);

/*
 * Kernel debug log functions
 */
int encos_enable_kdbg(void);
int encos_disable_kdbg(void);

/*
 * Anonymous memory backend support
 */
void *encos_event_futex_alloc(size_t size);
void encos_event_futex_free(void *handle, size_t size);

/*
 * Testcase functions
 */
int test_mmap(void);
#endif