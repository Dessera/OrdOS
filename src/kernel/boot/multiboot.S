#include "ordos/kernel/config.h"
#include "ordos/kernel/boot/multiboot.h"

.section .text.init, "ax"

  .globl _start
_start:
  movl $(_boot_stack + ORDOS_INIT_STACKSIZE), %esp

  // clear eflags
  pushl $0
  popf

  // call c loader with magic and information ptr
  pushl %ebx
  pushl %eax
  call kprelude

  // fix stack pointer
  addl $ORDOS_KERNEL_VADDR, %esp

  // call kernel main function
  call kmain

_start_hlt:
  hlt
  jmp _start_hlt

.section .rodata.init.multiboot, "a"
  MULTIBOOT_HEADER(_multiboot_header, MULTIBOOT_HEADER_ARCH_1386)
    MULTIBOOT_HEADER_TAG_EFI32_ENTRY(_multiboot_efi32_entry, _start)
    MULTIBOOT_HEADER_TAG_BIOS_ENTRY(_multiboot_bios_entry, _start)
    MULTIBOOT_HEADER_TAG_END(_multiboot_end)
  MULTIBOOT_HEADER_END(_multiboot_header)

.section .bss.init, "wa"
_boot_stack:
  .zero ORDOS_INIT_STACKSIZE
