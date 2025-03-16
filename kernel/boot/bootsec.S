#include "kernel/config/boot.h"
#include "kernel/boot.h"
#include "kernel/info.h"

.code16
.section ".bstext", "ax"
_bootsec_start:
  // set up segment registers
  movw %cs, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movw $BOOT_MBR_START, %sp

  // disable interrupts because they're not ready
  cli

  movw $_bsd_welcome, %si
  call _bprint

  movw $_bsd_loading_indicator, %si
  call _bprint

  // read loader to mem
_bs_load_ldr:
  movw $0x80, %dx
  movw $BOOT_LDR_SEC_START + 1, %cx
  movw $BOOT_LDR_START, %bx
_bs_load_ldr_retry:
  movb $0x02, %ah
  movb $BOOT_LDR_SIZE, %al
  int $0x13
  jnc _bs_load_ldr_done
_bs_load_ldr_error:
  movw $_bsd_retry_indicator, %si
  call _bprint
  jmp _bs_load_ldr_retry
_bs_load_ldr_done:
  jmp _ldr_start

/*
 * Print string to screen
 *
 * Parameters:
 *    %si = string addr
 */
  .globl _bprint
_bprint:
  pusha
  cld
  movw $0x07, %bx
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

_bsd_welcome:
  .ascii KFULLNAME_STR, " STEP 1\n\r\0"
_bsd_loading_indicator:
  .asciz "Loading STEP 2...\n\r"
_bsd_retry_indicator:
  .asciz "Failed to load STEP 2, retrying...\n\r"

.org BOOT_SEC_SIZE - 2
boot_flag: .word 0xaa55

