/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* 
 * Chuqi: libos_handle type ENCOS support
 */

#pragma once

#include "libos_handle.h"
#include "libos_fs.h"

#define LIBOS_ENCOS_DEV "/dev/encos-dev"

/*
 * To compatible with fs backends
 */
struct libos_encos_ops {
    /* mount: mount an uri to the certain location */
    int (*mount)(struct libos_mount_params* params, void** mount_data);
    int (*unmount)(void* mount_data);
    
    /* Open the device file */
    int (*open)(struct libos_handle *handle);
    /*
     * \brief Map file at an address.
     *
     * \param hdl     File handle.
     * \param addr    Address of the memory region. Cannot be NULL.
     * \param size    Size of the memory region.
     * \param prot    Permissions for the memory region (`PROT_*`).
     * \param flags   `mmap` flags (`MAP_*`).
     * \param offset  Offset in file.
     *
     * Maps the file at given address. This might involve mapping directly (`PalStreamMap`), or
     * mapping anonymous memory (`PalVirtualMemoryAlloc`) and writing data.
     *
     * `addr`, `offset` and `size` must be alloc-aligned (see `IS_ALLOC_ALIGNED*` macros in
     * `libos_internal.h`).
     * 
     * IMPORTANT: encos driver mmap's offset is a magic number, which is 
     * used for identifying the shared-memory ID (based on enclave ID).
     */
    int (*mmap)(struct libos_handle *handle, void *addr, size_t size, int prot, int flags, 
                uint64_t offset);
    
    int (*ioctl)(struct libos_handle *handle, unsigned int cmd, unsigned long arg);

    /* close: clean up the file state inside the handle */
    int (*close)(struct libos_handle* hdl);
};