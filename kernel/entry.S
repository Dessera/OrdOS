#include "kernel/config/memory.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/gdt.h"

.section ".text"
.globl _start
_start:
    lgdt MEM_KERNEL_PADDR(_gdt_ptr)

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
  .long MEM_KERNEL_PADDR(gdt)