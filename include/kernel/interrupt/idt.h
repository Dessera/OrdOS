#pragma once

#include "kernel/types.h"

#define IDT_SIZE 0x21
#define IDT_TYPE_INTR 0xe
#define IDT_TYPE_TRAP 0x6

#define IDT_DPL_KERNEL 0
#define IDT_DPL_USER 3

#define IDT_DESC_ATTR(p, dpl, type) ((p << 7) | (dpl << 5) | (type))

struct idt_desc_t
{
  u16 offs_lb;
  u16 sel;
  u8 nargs;
  u8 attr;
  u16 offs_hb;
} __attribute((packed));

void
init_idt(void);

void
intr_common_handler(u32 irq);
