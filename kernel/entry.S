#include "kernel/config/memory.h"
#include "kernel/memory/gdt.h"

.section ".text"
.globl _start
_start:
    lgdt _gdt_ptr - MEM_KERNEL_VSTART

    movw $GDT_KDATA_SELECTOR, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    movl $MEM_KERNEL_STACK_VSTART, %esp
    xorl %ebp, %ebp

    ljmp $GDT_KCODE_SELECTOR, $kmain

_gdt_ptr:
  .word MEM_GDT_SIZE * 8 - 1
  .long gdt - MEM_KERNEL_VSTART