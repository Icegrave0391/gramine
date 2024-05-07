#ifndef __ENCOS_COMMON_H__
#define __ENCOS_COMMON_H__

/* IOCTL-related headers */
#include <linux/ioctl.h>
#include <sys/ioctl.h>

#define ENCOS_ENCLAVE_REQUEST   _IOW('m', 1, unsigned int)

#define ENCOS_ENABLE_KDBG       _IOW('m', 2, unsigned int)
#define ENCOS_DISABLE_KDBG      _IOW('m', 3, unsigned int)
#define ENCOS_ENCLAVE_ACT       _IOW('m', 4, unsigned int)
#define ENCOS_ENCLAVE_EXIT      _IOW('m', 5, unsigned int)


#endif