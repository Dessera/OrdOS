#define INTR_ERR nop
#define INTR_NOERR pushl $0

#define INTR_VEC_ID(index) idt_##index##_desc

#define INTR_VEC_ENTRY(index, err)                                             \
.section ".text";                                                              \
INTR_VEC_ID(index):;                                                           \
  err;                                                                         \
  push %ds;                                                                    \
  push %es;                                                                    \
  push %fs;                                                                    \
  push %gs;                                                                    \
  pushal;                                                                      \
  movb $0x20, %al;                                                             \
  outb %al, $0x0a;                                                             \
  outb %al, $0x20;                                                             \
  pushl $index;                                                                \
  call common_intr_handler;                                                    \
  addl $0x04, %esp;                                                            \
  jmp _asm_intr_exit;                                                          \
.section ".data";                                                              \
  .long INTR_VEC_ID(index)

.section ".data"

_asm_intr_label:
  .asciz "intr occurred\n"

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
INTR_VEC_ENTRY(0x20, INTR_NOERR)

.section ".text"
_asm_intr_exit:
  popal
  pop %gs
  pop %fs
  pop %es
  pop %ds
  addl $0x04, %esp
  iret
