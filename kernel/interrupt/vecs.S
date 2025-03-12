#include "kernel/device/pci.h"

#define INTR_ERR nop
#define INTR_NOERR pushl $0

#define INTR_VEC_ID(index) idt_##index##_desc

#define INTR_VEC_ENTRY(index, err)                                             \
.section ".text";                                                              \
INTR_VEC_ID(index):;                                                           \
  err;                                                                         \
  pushl $index;                                                                \
  jmp _asm_intr_common_handler;                                                \
.section ".data";                                                              \
  .long INTR_VEC_ID(index)

.section ".data"
.globl _asm_intr_vecs
_asm_intr_vecs:
INTR_VEC_ENTRY(0x00, INTR_NOERR)
INTR_VEC_ENTRY(0x01, INTR_NOERR)
INTR_VEC_ENTRY(0x02, INTR_NOERR)
INTR_VEC_ENTRY(0x03, INTR_NOERR)
INTR_VEC_ENTRY(0x04, INTR_NOERR)
INTR_VEC_ENTRY(0x05, INTR_NOERR)
INTR_VEC_ENTRY(0x06, INTR_NOERR)
INTR_VEC_ENTRY(0x07, INTR_NOERR)
INTR_VEC_ENTRY(0x08, INTR_ERR)
INTR_VEC_ENTRY(0x09, INTR_NOERR)
INTR_VEC_ENTRY(0x0a, INTR_ERR)
INTR_VEC_ENTRY(0x0b, INTR_ERR)
INTR_VEC_ENTRY(0x0c, INTR_NOERR)
INTR_VEC_ENTRY(0x0d, INTR_ERR)
INTR_VEC_ENTRY(0x0e, INTR_ERR)
INTR_VEC_ENTRY(0x0f, INTR_NOERR)
INTR_VEC_ENTRY(0x10, INTR_NOERR)
INTR_VEC_ENTRY(0x11, INTR_ERR)
INTR_VEC_ENTRY(0x12, INTR_NOERR)
INTR_VEC_ENTRY(0x13, INTR_NOERR)
INTR_VEC_ENTRY(0x14, INTR_NOERR)
INTR_VEC_ENTRY(0x15, INTR_NOERR)
INTR_VEC_ENTRY(0x16, INTR_NOERR)
INTR_VEC_ENTRY(0x17, INTR_NOERR)
INTR_VEC_ENTRY(0x18, INTR_ERR)
INTR_VEC_ENTRY(0x19, INTR_NOERR)
INTR_VEC_ENTRY(0x1a, INTR_ERR)
INTR_VEC_ENTRY(0x1b, INTR_ERR)
INTR_VEC_ENTRY(0x1c, INTR_NOERR)
INTR_VEC_ENTRY(0x1d, INTR_ERR)
INTR_VEC_ENTRY(0x1e, INTR_ERR)
INTR_VEC_ENTRY(0x1f, INTR_NOERR)

// IRQs
INTR_VEC_ENTRY(0x20, INTR_NOERR)  // timer
INTR_VEC_ENTRY(0x21, INTR_NOERR)  // keyboard
INTR_VEC_ENTRY(0x22, INTR_NOERR)
INTR_VEC_ENTRY(0x23, INTR_NOERR)
INTR_VEC_ENTRY(0x24, INTR_NOERR)
INTR_VEC_ENTRY(0x25, INTR_NOERR)
INTR_VEC_ENTRY(0x26, INTR_NOERR)
INTR_VEC_ENTRY(0x27, INTR_NOERR)
INTR_VEC_ENTRY(0x28, INTR_NOERR)
INTR_VEC_ENTRY(0x29, INTR_NOERR)
INTR_VEC_ENTRY(0x2a, INTR_NOERR)
INTR_VEC_ENTRY(0x2b, INTR_NOERR)
INTR_VEC_ENTRY(0x2c, INTR_NOERR)
INTR_VEC_ENTRY(0x2d, INTR_NOERR)
INTR_VEC_ENTRY(0x2e, INTR_NOERR)
INTR_VEC_ENTRY(0x2f, INTR_NOERR)

.section ".text"

_asm_intr_common_handler:
  pushl %ds
  pushl %es
  pushl %fs
  pushl %gs
  pushal
  // get index and repush it for later
  movl 0x30(%esp), %eax
  pushl %eax
  movb $PIC_EOI, %al
  outb %al, $PIC_MASTER_CMD
  outb %al, $PIC_SLAVE_CMD

  // call c handler with intr index
  call intr_common_handler

.globl _asm_intr_common_handler_exit
_asm_intr_common_handler_exit:
  // pop index
  addl $0x04, %esp
  
  popal
  popl %gs
  popl %fs
  popl %es
  popl %ds

  // pop index & err
  addl $0x08, %esp
  iret
