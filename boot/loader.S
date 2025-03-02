#include "kernel/boot.h"
#include "kernel/info.h"
#include "kernel/mem.h"
#include "kernel/disk.h"
#include "kernel/device/vga.h"

// print before protected mode
#define LPRINT(str) movw str, %si; call _lprint

// print after protected mode
#define PPRINT(line, str) movl line, %eax; movl str, %esi; call _pprint

.code16
.section ".ldrtext", "ax"
  // gdt table
_gdt_table:
  GDT_DESC_NULL()
  GDT_DESC_CODE()
  GDT_DESC_DATA()
  GDT_DESC_VIDEO()
_gdt_end:

_gdt_size:
  .word _gdt_end - _gdt_table
_gdt_table_ptr:
  .word _gdt_end - _gdt_table - 1
  .long _gdt_table

  // ards buffer 244 bytes
_ards_buf:
  .space 0xf4
_ards_nr:
  .word 0

.org LDRENTRY_OFFSET
.globl _start
_start:
  // check memory

  LPRINT($_lb_mem_indicator)

  xorl %ebx, %ebx
  movl $E820MAGIC, %edx
  movw $_ards_buf, %di
_ldr_fetch_ards:
  movl $E820FUNC, %eax
  movl $0x14, %ecx
  int $0x15
  jc _ldr_mem_error
  addw %cx, %di
  incw _ards_nr
  cmpl $0x00, %ebx
  jnz _ldr_fetch_ards

  movw (_ards_nr), %cx
  movl $_ards_buf, %ebx
  xorl %edx, %edx
_ldr_find_mem:
  movl (%ebx), %eax
  addl 8(%ebx), %eax
  addl $0x14, %ebx
  cmpl %eax, %edx
  jge _ldr_next_mem
  movl %eax, %edx
_ldr_next_mem:
  loop _ldr_find_mem
  jmp _ldr_mem_ok

_ldr_mem_error:

  LPRINT($_lb_mem_error)

  jmp _ldr_error_hlt
_ldr_mem_ok:

  // enable A20
  inb $0x92, %al
  orb $0x02, %al
  outb %al, $0x92

  // load gdt
  lgdt _gdt_table_ptr

  // enable protected mode
  movl %cr0, %eax
  orl $0x01, %eax
  movl %eax, %cr0

  // jump to _start_protected
  ljmp $GDT_CODE_SELECTOR, $_start_protected

_ldr_error_hlt:
  hlt
  jmp _ldr_error_hlt

/*
 * Print string to screen
 *
 * Parameters:
 *    %si = string addr
 */
_lprint:
  pusha
  cld
  movw $0x07, %bx
_lprint_loop:
  lodsb
  andb %al, %al
  jz _lprint_done
  movb $0x0e, %ah
  int $0x10
  jmp _lprint_loop
_lprint_done:
  popa
  ret

.code32
_start_protected:
  movw $GDT_DATA_SELECTOR, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movl $LDRSEC_ADDR, %esp

  call _pclear

  PPRINT($0x00, $_pb_welcome)

  PPRINT($0x01, $_pb_page_indicator)

_prot_page_setup:
  // clear page table
  movl $PAGE_TABLE_ADDR, %edi
  movl $PAGE_SIZE, %ecx
  movb $0x00, %al
  
  cld
  rep stosb

_prot_page_init:
  // map first 4MB
  movl $PDE_DESC(INDEX_PAGE_TABLE_ADDR(1), 1, 1, 1), (PAGE_TABLE_ADDR)
  movl $PDE_DESC(INDEX_PAGE_TABLE_ADDR(1), 1, 1, 1), (PAGE_TABLE_ADDR + PDE_KERNEL_OFFSET)

  // map page table itself
  movl $PDE_DESC(INDEX_PAGE_TABLE_ADDR(0), 1, 1, 1), (PAGE_TABLE_ADDR + PAGE_SIZE - 0x04)

  // map kernel storage
_prot_kernel_page_init:
  movl $0x100, %ecx
  movl $PDE_DESC(0, 1, 1, 1), %eax
  movl $INDEX_PAGE_TABLE_ADDR(1), %edi
_prot_kernel_page_loop:
  movl %eax, (%edi)
  addl $PAGE_SIZE, %eax
  addl $0x04, %edi
  loop _prot_kernel_page_loop

  // map kernel memory
_prot_kernel_mem_init:
  movl $PDE_DESC(INDEX_PAGE_TABLE_ADDR(2), 1, 1, 1), %eax
  movl $PAGE_TABLE_ADDR + PDE_KERNEL_OFFSET + 0x04, %edi
_prot_kernel_mem_loop:
  movl %eax, (%edi)
  addl $PAGE_SIZE, %eax
  addl $0x04, %edi
  cmpl $PAGE_TABLE_ADDR + PAGE_SIZE - 0x04, %edi
  jne _prot_kernel_mem_loop

  // update gdt
  sgdt _gdt_table_ptr

  movl (_gdt_table_ptr + 2), %ebx
  orl $KERNELBASE, GDT_VIDEO_SELECTOR + 4(%ebx)

  addl $KERNELBASE, (_gdt_table_ptr + 2)
  addl $KERNELBASE, %esp

  // load page table
  movl $PAGE_TABLE_ADDR, %eax
  movl %eax, %cr3

  movl %cr0, %eax
  orl $0x80000000, %eax
  movl %eax, %cr0

  lgdt _gdt_table_ptr

  // load kernel
  PPRINT($0x02, $_pb_kernel_indicator)
  movl $KERNELSIZE, %ecx
  movl $KERNELSTART, %eax
  movl $KERNELENTRY_ADDR, %edi
  call _prot_load_kernel

  // jump to kernel
  movl $KERNELBASE + 0x9f000, %esp
  call kmain

_prot_error_hlt:
  hlt
  jmp _prot_error_hlt

/*
 * Clear screen
 */
_pclear:
  pushl %eax
  pushw %es
  movw $GDT_VIDEO_SELECTOR, %ax
  movw %ax, %es
  movl $0x0, %edi
  movl $VGA_BUF_WIDTH * VGA_BUF_HEIGHT, %ecx
  movw $0x0720, %ax

  cld
  rep stosw

  popw %ax
  movw %ax, %es
  popl %eax
  ret

/*
 * Print string to screen
 *
 * Parameters:
 *    %esi = string addr
 *    %eax = line number
 */
_pprint:
  pushal
  cld

  movl $VGA_BUF_WIDTH * 2, %ecx
  mull %ecx
  movl %eax, %edx
  movw $GDT_VIDEO_SELECTOR, %ax
  movw %ax, %gs

  movl $0x0, %ecx
_pprint_loop:
  lodsb
  andb %al, %al
  jz _pprint_done
  movb $0x0f, %ah
  movw %ax, %gs:(%edx, %ecx, 2)
  incl %ecx
  jmp _pprint_loop
_pprint_done:
  popal
  ret

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

.section ".ldrdata", "a"
_lb_mem_indicator:
  .asciz "Fetching memory info\n\r"
_lb_mem_error:
  .asciz "Failed to fetch memory info\n\r"
_pb_welcome:
  .ascii KFULLNAME_STR, " STEP 2\0"
_pb_page_indicator:
  .asciz "Setup page table"
_pb_kernel_indicator:
  .asciz "Loading kernel"
