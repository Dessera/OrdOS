#include "kernel/config/boot.h"
#include "kernel/config/memory.h"
#include "kernel/boot.h"
#include "kernel/info.h"
#include "kernel/memory/gdt.h"
#include "kernel/device/disk.h"

#define E820MAGIC 0x534d4150
#define E820FUNC 0xe820

.code16
.section ".ldrtext", "ax"

  .globl _ldr_start
_ldr_start:
  xorl %ebx, %ebx

  movw $_ld_welcome, %si
  call _bprint

  movw $_ld_mem_indicator, %si
  call _bprint

  // fetch memory info
  movl $E820MAGIC, %edx
  movw $_asm_mem_zone, %di
_ldr_fetch_ards:
  movl $E820FUNC, %eax
  movl $0x14, %ecx
  int $0x15

  jc _ldr_mem_error
  addw %cx, %di
  incw _asm_mem_nr
  cmpl $0x00, %ebx
  jnz _ldr_fetch_ards
  jmp _ldr_mem_ok

_ldr_mem_error:

  movw $_ld_mem_error, %si
  call _bprint

  jmp _ldr_error_hlt

_ldr_mem_ok:

  movw $_ld_kernel_indicator, %si
  call _bprint

  // enable A20
  inb $0x92, %al
  orb $0x02, %al
  outb %al, $0x92

  // load boot gdt
  lgdt _ld_gdt_ptr

  // enable protected mode
  movl %cr0, %eax
  orl $0x01, %eax
  movl %eax, %cr0

  ljmp $GDT_KCODE_SELECTOR, $_prot_start

_ldr_error_hlt:
  hlt
  jmp _ldr_error_hlt

.globl _asm_mem_zone
_asm_mem_zone:
  .space MEM_E820_SIZE * 0x14

.globl _asm_mem_nr
_asm_mem_nr:
  .word 0

_ld_gdt_ptr:
  .word MEM_BOOT_GDT_SIZE * 8 - 1
  .long boot_gdt

_ld_welcome:
  .ascii KFULLNAME_STR, " STEP 2\n\r\0"
_ld_mem_indicator:
  .asciz "Fetching memory info...\n\r"
_ld_mem_error:
  .asciz "Failed to fetch memory info\n\r"
_ld_kernel_indicator:
  .asciz "Loading kernel...\n\r"

.code32
_prot_start:
  movw $GDT_KDATA_SELECTOR, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movw %ax, %ss

  // reset stack pointer
  movl $BOOT_MBR_START, %esp

  // load kernel
  movl $BOOT_KERNEL_SIZE, %ecx
  movl $BOOT_KERNEL_SEC_START, %eax
  movl $BOOT_KERNEL_START, %edi
  call _prot_load_kernel

  // get real addr of kernel entry (because page table is not set up yet)
  call _start - MEM_KERNEL_VSTART

_prot_error_hlt:
  hlt
  jmp _prot_error_hlt

/*
 * Load sectors from disk
 *
 * Parameters:
 *   %ecx = number of sectors
 *   %eax = LBA address
 *   %edi = destination buffer
 */
_prot_load_kernel:
  pushal

  call _prot_disk_wait

  // store number of sectors
  pushl %eax
  movw $DISK_CMD_SECS, %dx
  movb %cl, %al
  outb %al, %dx
  popl %eax

  // store LBA address
  movw $DISK_CMD_LBALOW, %dx
  outb %al, %dx

  movw $DISK_CMD_LBAMID, %dx
  shrl $8, %eax
  outb %al, %dx

  movw $DISK_CMD_LBAHIGH, %dx
  shrl $8, %eax
  outb %al, %dx

  movw $DISK_CMD_LBAEX, %dx
  shrl $8, %eax
  andb $0x0f, %al
  orb $0xe0, %al
  outb %al, %dx

  // read command
  movw $DISK_CMD_CTRL, %dx
  movb $DISK_CMD_READ, %al
  outb %al, %dx

  call _prot_disk_wait

  // move data
  movl %ecx, %eax
  movl $0x80, %ebx
  mull %ebx
  movw $DISK_PORT_DATA, %dx
  movl %eax, %ecx
_prot_load_kernel_loop:
  call _prot_disk_wait
  inl %dx, %eax
  movl %eax, (%edi)
  addl $4, %edi
  loop _prot_load_kernel_loop

  popal
  ret

/*
 * Wait for disk to be ready
 */
_prot_disk_wait:
  pushw %dx
  pushw %ax
  movw $DISK_CMD_CTRL, %dx
_prot_disk_wait_loop:
  inb %dx, %al
  andb $DISK_STATUS_READY | DISK_STATUS_BUSY, %al
  cmpb $DISK_STATUS_READY, %al
  jne _prot_disk_wait_loop
  popw %ax
  popw %dx
  ret
