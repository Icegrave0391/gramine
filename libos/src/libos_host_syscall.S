/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* 
 * Chuqi: bad implementation of raw host syscalls.
 *        see libos_host_syscall.h 
 */

#define __ASSEMBLY__
#include <asm/unistd.h>

.global do_syscall
.type do_syscall, @function
.align 0x10
do_syscall:
    .cfi_startproc
    mov %rdi, %rax
    mov %rsi, %rdi
    mov %rdx, %rsi
    mov %rcx, %rdx
    mov %r8, %r10
    mov %r9, %r8
    mov 8(%rsp), %r9
    syscall
    ret
    .cfi_endproc

