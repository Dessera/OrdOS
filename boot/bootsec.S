#include "kernel/boot.h"
#include "kernel/gdt.h"
#include "kernel/info.h"

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
  call _bsu_print

  movw $_bsd_loading_indicator, %si
  call _bsu_print

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
  call _bsu_print
  jmp _bs_load_ldr_retry
_bs_load_ldr_done:
  ljmp $0x0, $LDRENTRY_ADDR

_bsu_print: # params : %si = string
  push %ax
  push %bx
  movw $0x0007, %bx
_bsu_print_loop:
  lodsb
  andb %al, %al
  jz _bsu_print_done
  movb $0x0e, %ah
  int $0x10
  jmp _bsu_print_loop
_bsu_print_done:
  pop %bx
  pop %ax
  ret

.org 0x180

.section ".bsdata", "a"

_bsd_welcome:
  .ascii KFULLNAME_STR, "\n\r\0"
_bsd_loading_indicator:
  .asciz "Loading kernel...\n\r"
_bsd_retry_indicator:
  .asciz "Failed to load kernel, retrying...\n\r"

.org 0x7E
boot_flag: .word 0xaa55

.section ".ldrtext", "ax"
_gdt_table:
  GDT_DESC_NULL()
  GDT_DESC_CODE()
  GDT_DESC_DATA()
_gdt_table_ptr:
  .word . - _gdt_table - 1
  .long _gdt_table

.org LDRENTRY_OFFSET
.globl _start
_start:
  movw $_ldr_enter_protected_mode_indicator, %si
  call _ldr_print

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

_ldr_print: # params : %si = string
  push %ax
  push %bx
  movw $0x0007, %bx
_ldr_print_loop:
  lodsb
  andb %al, %al
  jz _ldr_print_done
  movb $0x0e, %ah
  int $0x10
  jmp _ldr_print_loop
_ldr_print_done:
  pop %bx
  pop %ax
  ret

.code32
_start_protected:
  movw $GDT_DATA_SELECTOR, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  mov $LDRSEC_ADDR, %esp

  /* Terminate boot process */
  jmp .

.section ".ldrdata", "a"

_ldr_enter_protected_mode_indicator:
  .asciz "Entering protected mode...\n\r"

.org 0x800