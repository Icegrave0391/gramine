/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * This file contains code for implementation of 'encos' filesystem.
 * If mounted in manifest, file is to communicate to the encos-dev.
 */


#include "libos_flags_conv.h"
#include "libos_fs.h"
#include "libos_handle.h"
#include "libos_lock.h"
#include "linux_abi/errors.h"
#include "perm.h"
#include "stat.h"

#include "libos_host_syscall.h"
#include "pal_encos_driver.h"


#define HOST_PERM(perm) ((perm) | PERM_r________)

static const char* strip_prefix(const char* uri) {
    const char* s = strchr(uri, ':');
    assert(s);
    return s + 1;
}

static int encos_mount(struct libos_mount_params* params, void** mount_data) {
    __UNUSED(mount_data);
    if (!params->uri) {
        log_error("Missing shared memory URI");
        return -EINVAL;
    }
    if (!strstartswith(params->uri, URI_PREFIX_DEV)) {
        log_error("'%s' is invalid shared memory URI (expected prefix %s)", params->uri,
                  URI_PREFIX_DEV);
        return -EINVAL;
    }

    log_always("Mount encos dev.");
    return 0;
}

int encos_mmap(struct libos_handle* hdl, void* addr, size_t size, int prot, int flags,
                      uint64_t offset) {
    /* both encos and trusted_shm can mmap */
    assert(hdl->type == TYPE_ENCOS || hdl->type == TYPE_SHM);
    assert(addr);

    // pal_prot_flags_t pal_prot = LINUX_PROT_TO_PAL(prot, flags);

    if (flags & MAP_ANONYMOUS)
        return -EINVAL;
    log_always("SHMMAP addr=0x%lx, offset=0x%lx.", (unsigned long)addr, (unsigned long)offset);

    /* 
     * directly ask driver to mmap shared memory 
     */
    /* 
     * TODO: In principle; the shared mmap should be either
     * R+X; or WO; but now RW
     * 
     * The security monitor should regulate only one owner (WO)
     * and the remaining parties are R+X
     */
    /*
     * The `offset` should be filled as its enclave ID.
     * We simply use 1 for now
     */
    assert(offset == 0);
    offset = 1;

    int ret = encos_shm_mmap(addr, size, prot, flags, offset);
    if (ret < 0)
        return ret;

    return 0;
}

/* Open a PAL handle, and associate it with a LibOS handle. */
static int encos_do_open(struct libos_handle* hdl, struct libos_dentry* dent, mode_t type,
                       int flags, mode_t perm) {
    assert(locked(&g_dcache_lock));

    char* uri;
    int ret = chroot_dentry_uri(dent, type, &uri);
    if (ret < 0)
        return ret;

    // PAL_HANDLE palhdl;
    // mode_t host_perm = HOST_PERM(perm);

    if (ret < 0) {
        ret = pal_to_unix_errno(ret);
        goto out;
    }

    log_always("encos_do_open: uri=%s, type=0x%x, flags=0x%x, perm=0x%x", 
                uri, type, flags, perm);
    hdl->uri = uri;    // should be dev:/dev/encos-dev
    uri = NULL;

    hdl->type = TYPE_ENCOS;
    hdl->pal_handle = NULL;

    // do nothing here
    /* todo: create encos_ops */ // not necessary
    ret = 0;

out:
    free(uri);
    return ret;
}

static int encos_setup_dentry(struct libos_dentry* dent, mode_t type, mode_t perm,
                              file_off_t size) {
    assert(locked(&g_dcache_lock));
    assert(!dent->inode);

    struct libos_inode* inode = get_new_inode(dent->mount, type, perm);
    if (!inode)
        return -ENOMEM;
    inode->size = size;
    dent->inode = inode;
    return 0;
}

/* called at the initialization to lookup and mount file */
int encos_lookup(struct libos_dentry* dent) {
    assert(locked(&g_dcache_lock));
    assert(dent->mount);
    assert(dent->mount->uri);

    const char *raw_uri; // the actual file path
    /*
     * we SHOULD know this specific encos file type yet,
     * Simply construct our raw_uri="/dev/encos-dev"
     */
    raw_uri = strip_prefix(dent->mount->uri);

    /*
     * simply fetch the file stats 
     */
    log_always("fd lookup: %d.\n", encos_fd());

    log_always("lookup raw uri: %s.", raw_uri);
    struct stat stat_buf;
    // replace to some pal functions
    // int ret = DO_SYSCALL(stat, raw_uri, &stat_buf);
    int ret = encos_dev_stat(&stat_buf);
    if (ret < 0) {
        log_error("encos_lookup file %s stat failed.\n", raw_uri);
        goto out;
    }
        
    /* We know the type is an available IFCHR (device) */
    if (!(S_ISCHR(stat_buf.st_mode))) {
        log_error("encos_lookup file %s is not a device file (res=%u).\n", 
                    raw_uri, stat_buf.st_mode);
        goto out;
    }
#define TEMP_SHARE_MASK     07777
    ret = encos_setup_dentry(dent, S_IFCHR, stat_buf.st_mode & TEMP_SHARE_MASK, stat_buf.st_size);
#undef TEMP_SHARE_MASK
out:
    return ret;
}

static int encos_open(struct libos_handle* hdl, struct libos_dentry* dent, int flags) {
    assert(locked(&g_dcache_lock));
    assert(dent->inode);

    return encos_do_open(hdl, dent, dent->inode->type, flags, /*perm=*/0);
}

/* ignore the truncate */
static int encos_truncate(struct libos_handle* hdl, file_off_t size)
{
    assert(hdl->type == TYPE_ENCOS);
    return 0;
}

struct libos_fs_ops encos_fs_ops = {
    .mount      = encos_mount,
    /* .read and .write are intentionally not supported to the encos dev. */
    .mmap       = encos_mmap,
    .hstat      = generic_inode_hstat,
    .truncate   = encos_truncate,
};

struct libos_d_ops encos_d_ops = {
    .open    = encos_open,
    .lookup  = encos_lookup,
    // .creat   = shm_creat,
    .stat    = generic_inode_stat,
    .unlink  = chroot_unlink,
};

struct libos_fs encos_builtin_fs = {
    .name   = "encos_dev",
    .fs_ops = &encos_fs_ops,
    .d_ops  = &encos_d_ops,
};