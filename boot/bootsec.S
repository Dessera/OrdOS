#include "kernel/config/boot.h"
#include "kernel/boot.h"
#include "kernel/info.h"

// print before protected mode
#define BPRINT(str) movw str, %si; call _bprint

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

  BPRINT($_bsd_welcome)
  BPRINT($_bsd_loading_indicator)

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
  BPRINT($_bsd_retry_indicator)
  jmp _bs_load_ldr_retry
_bs_load_ldr_done:
  jmp _start

/*
 * Print string to screen
 *
 * Parameters:
 *    %si = string addr
 */
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

.org 0x180

.section ".bsdata", "a"

_bsd_welcome:
  .ascii KFULLNAME_STR, " STEP 1\n\r\0"
_bsd_loading_indicator:
  .asciz "Loading STEP 2...\n\r"
_bsd_retry_indicator:
  .asciz "Failed to load STEP 2, retrying...\n\r"

.org 0x7e
boot_flag: .word 0xaa55

