#include "kernel/boot.h"
#include "kernel/gdt.h"
#include "kernel/info.h"
#include "kernel/mem.h"

.code16
.section ".bstext", "ax"
_bootsec_start:
  /* set up segment registers */
  movw %cs, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movw $BOOTSEC_ADDR, %sp

  /* disable interrupts because they're not ready */
  cli
  cld

  movw $_bsd_welcome, %si
  call _bprint

  movw $_bsd_loading_indicator, %si
  call _bprint

  /* read loader to mem */
_bs_load_ldr:
  movw $0x0080, %dx
  movw $LDRSTART, %cx
  movw $LDRSEC_ADDR, %bx
_bs_load_ldr_retry:
  movb $0x02, %ah
  movb $LDRSIZE, %al
  int $0x13
  jnc _bs_load_ldr_done
_bs_load_ldr_error:
  movw $_bsd_retry_indicator, %si
  call _bprint
  jmp _bs_load_ldr_retry
_bs_load_ldr_done:
  ljmp $0x0, $LDRENTRY_ADDR


_bprint: # params : %si = string
  pusha
  movw $0x0007, %bx
_bprint_loop:
  lodsb
  andb %al, %al
  jz _bprint_done
  movb $0x0e, %ah
  int $0x10
  jmp _bprint_loop
_bprint_done:
  popa
  ret

.org 0x180

.section ".bsdata", "a"

_bsd_welcome:
  .ascii KFULLNAME_STR, "\n\r\0"
_bsd_loading_indicator:
  .asciz "Loading STEP 2...\n\r"
_bsd_retry_indicator:
  .asciz "Failed to load STEP 2, retrying...\n\r"

.org 0x7E
boot_flag: .word 0xaa55

.section ".ldrtext", "ax"
  /* gdt table */
_gdt_table:
  GDT_DESC_NULL()
  GDT_DESC_CODE()
  GDT_DESC_DATA()
_gdt_end:

_gdt_size:
  .word _gdt_end - _gdt_table
_gdt_table_ptr:
  .word _gdt_end - _gdt_table - 1
  .long _gdt_table

  /* ards buffer 244 bytes */
_ards_buf:
  .space 244
_ards_nr:
  .word 0

.org LDRENTRY_OFFSET
.globl _start
_start:
  movw $_lb_welcome, %si
  call _lprint

  /* check memory */
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
  cmpl $0x0, %ebx
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
  movw $_lb_mem_error, %si
  call _lprint
  jmp _ldr_error_hlt
_ldr_mem_ok:
  movw $_lb_mem_ok, %si
  call _lprint

  /* enable A20 */
  inb $0x92, %al
  orb $0x02, %al
  outb %al, $0x92

  /* load gdt */
  lgdt _gdt_table_ptr

  /* enable protected mode */
  movl %cr0, %eax
  orl $0x01, %eax
  movl %eax, %cr0

  /* jump to _start_protected */
  ljmp $GDT_CODE_SELECTOR, $_start_protected

_ldr_error_hlt:
  hlt
  jmp _ldr_error_hlt

_lprint: # params : %si = string
  pusha
  movw $0x0007, %bx
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

  movl $0x01, %eax
  movl $_prot_welcome, %esi
  call _pprint

  /* Terminate boot process */
  jmp .

#include "asm/pscreen.inc"

.section ".ldrdata", "a"
_lb_welcome:
  .asciz "STEP 2 loaded\n\r"
_lb_mem_error:
  .asciz "Failed to fetch memory info\n\r"
_lb_mem_ok:
  .asciz "Memory info fetched\n\r"
_prot_welcome:
  .asciz "In protected mode"
