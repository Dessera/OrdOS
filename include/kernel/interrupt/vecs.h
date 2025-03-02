#pragma once

#define INTR_ERR nop;
#define INTR_NOERR push $0;

#define INTR_VEC_ID(index) idt_##index##_desc

// clang-format off
#define INTR_VEC_ENTRY(index, err)                                             \
.section ".text";                                                              \
.globl INTR_VEC_ID(index);                                                     \
INTR_VEC_ID(index):;                                                           \
  err;                                                                         \
  movb $0x20, % al;                                                            \
  outb % al, $0x0a;                                                            \
  outb % al, $0x20;                                                            \
  add $0x04, % esp;                                                            \
  iret;                                                                        \
.section ".data";                                                              \
  .long INTR_VEC_ID(index)
// clang-format on