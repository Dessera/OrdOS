#include "kernel/boot.h"
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
  sti
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
.gdt_table:

.globl _start
_start:
  /* reset segment registers for current cs */

  movw $_ldr_msg, %si
  call _ldr_print

_ldr_end:
  hlt
  jmp _ldr_end

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

.section ".ldrdata", "a"

_ldr_msg:
  .asciz "Here is unimplemented loader\n\r"

.org 0x800