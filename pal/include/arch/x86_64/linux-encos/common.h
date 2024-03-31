#ifndef __ENCOS_COMMON_H__
#define __ENCOS_COMMON_H__

/* IOCTL-related headers */
#include <linux/ioctl.h>
#include <sys/ioctl.h>

#define ENCOS_ENCLAVE_REQUEST   _IOW('m', 1, unsigned int)

#endif