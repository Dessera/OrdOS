
#include "asm/boot.h"
#include "kernel/info.h"

.code16
.section ".bstext", "ax"

ljmp $BOOTSEC, $_bootsec_start

_bootsec_start:
  /* set up segment registers */
  movw %cs, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  xorw %sp, %sp
  sti
  cld

  /* clear screen */
  movw $0x0600, %ax
  movw $0x0700, %bx
  xorw %cx, %cx
  movw $0x184f, %di
  int $0x10

  movw $_bsd_welcome, %si
  call _bsu_print

  movw $_bsd_loading_indicator, %si
  call _bsu_print

  movw $LDRSEC, %ax
  movw %ax, %es

  /* read loader to mem */
  movw $0x0080, %dx

  movw $0x0002, %cx   # from sector 2
  movw $0x0204, %ax   # read 4 sectors
  movw $0x0000, %bx   # buffer
  int $0x13

  # restore es
  movw %cs, %ax
  movw %ax, %es

  ljmp $LDRENTRY, $0x0000

_bsu_print: # params : %si = string
  push %ax
  push %bx
  movw $0x7, %bx
_bsu_print_loop:
  lodsb
  andb %al, %al
  jz _bsu_print_done
  movb $0xe, %ah
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

.org 0x7E
boot_flag: .word 0xAA55

.section ".ldrtext", "ax"
.gdt_table:

.globl _start
_start:
  /* reset segment registers for current cs */
  movw %cs, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  xorw %sp, %sp

  movw $_ldr_msg, %si
  call _ldr_print

_ldr_end:
  hlt
  jmp _ldr_end

_ldr_print: # params : %si = string
  push %ax
  push %bx
  movw $0x7, %bx
_ldr_print_loop:
  lodsb
  andb %al, %al
  jz _ldr_print_done
  movb $0xe, %ah
  int $0x10
  jmp _ldr_print_loop
_ldr_print_done:
  pop %bx
  pop %ax
  ret

.section ".ldrdata", "a"

_ldr_msg:
  .asciz "Here is unimplemented loader\n\r"
