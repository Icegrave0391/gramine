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


int g_assign_futex = 0;

/* global encos_fd */
int g_encos_fd = -1;

int open_encos_driver(void) 
{
    /* open and initialize */
    if (g_encos_fd < 0) {
        g_encos_fd = DO_SYSCALL(open, ENCOS_DEV, O_RDWR);
    }

    if (g_encos_fd < 0) {
        if (g_encos_fd != -ENOENT)
            log_always("----Opened ENCOS driver (%s) failed. %d. %d", 
                        ENCOS_DEV, g_encos_fd, -ENOENT);
        return -1;
    }

#ifdef ENCOS_DEBUG
    log_always("Opened ENCOS driver (%s) to g_encos_fd=%d",
                ENCOS_DEV, g_encos_fd);
#endif
    /* Return that everything works */
    //pal_printf("[*] Opened device driver successfully.\n");
    return g_encos_fd;
}

int encos_dev_stat(void *buf)
{
    return DO_SYSCALL(stat, ENCOS_DEV, buf);
    
}

int encos_shm_mmap(void *addr, size_t size, int prot, int flags, uint64_t offset)
{
    int fd;
    fd = encos_fd();
    if (fd < 0) {
        log_error("Error: could not open ENCOS driver (%s)", ENCOS_DEV);
        return -ENOENT;
    }
    /* directly perform mmap */
    log_always("SHMMAP addr=0x%lx, offset=0x%lx, prot=0x%x.",
                 (unsigned long)addr, (unsigned long)offset, prot);
    void* mapped_addr = (void*)DO_SYSCALL(mmap, addr, size, prot,
                                          MAP_SHARED | MAP_FIXED_NOREPLACE, 
                                          fd, offset);
    if (IS_PTR_ERR(mapped_addr))
        return PTR_TO_ERR(mapped_addr);

    log_always("mmap addr=0x%lx, offset=0x%lx, returned_addr=0x%lx.",
                 (unsigned long)addr, (unsigned long)offset, (unsigned long)mapped_addr);
    assert(mapped_addr == addr);
    return 0;   
}

int encos_init_enclave(void)
{
    int ret;
    int fd = encos_fd();
    if (fd < 0) {
        // log_error("Error: could not open ENCOS driver (%s)", ENCOS_DEV);
        return -1;
    }
    /* ioctl */
    log_always("debug ioctl ENCOS_ENCLAVE_REQUEST");
    // ret = DO_SYSCALL(ioctl, fd, ENCOS_ENCLAVE_REQUEST, 0);
    return 0;
}

/* ============================
 * Kernel debug log functions
 * ============================ */
int encos_enable_kdbg(void)
{
    int fd, ret;
    fd = open_encos_driver();

    if (fd < 0) {
        // log_error("Error: could not open ENCOS driver (%s)", ENCOS_DEV);
        return -1;
    }

    ret = DO_SYSCALL(ioctl, fd, ENCOS_ENABLE_KDBG, 0);
    return ret;
}

int encos_disable_kdbg(void)
{
    int fd, ret;
    fd = open_encos_driver();

    if (fd < 0) {
        // log_error("Error: could not open ENCOS driver (%s)", ENCOS_DEV);
        return -1;
    }

    ret = DO_SYSCALL(ioctl, fd, ENCOS_DISABLE_KDBG, 0);
    return ret;
}

/* ============================
 * Anonymous memory backend support
 * ============================ */
void *encos_event_futex_alloc(size_t size)
{
    void *addr;
    int flags;
    int linux_prot;

    flags = MAP_ANONYMOUS | MAP_SHARED;
    linux_prot = PROT_READ | PROT_WRITE;

    addr = (void *)DO_SYSCALL(mmap, NULL, size, linux_prot, flags, -1, 0);
    if (IS_PTR_ERR(addr)) {
        return NULL;
    }
    memset(addr, 0, size);
    return addr;
}

void encos_event_futex_free(void *handle, size_t size)
{
    DO_SYSCALL(munmap, handle, size);
}


/* test function */
int test_mmap(void) 
{
    char *addr;
    int fd;
    log_always("Start test_mmap\n");
    // forcibly open
    fd = DO_SYSCALL(open, ENCOS_DEV, O_RDWR);
    if (fd < 0) {
        log_error("Error: could not open ENCOS driver (%s)\n", ENCOS_DEV);
        return -1;
    } else {
        log_always("test force-open ENCOS driver (%s) to fd=%d.\n", ENCOS_DEV, fd);
    }

    // fake mmap
    addr = (char *)DO_SYSCALL(mmap, NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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