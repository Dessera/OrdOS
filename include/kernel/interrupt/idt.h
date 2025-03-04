#pragma once

#include "kernel/assert.h"
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

KSTATIC_ASSERT_MSG(sizeof(struct idt_desc_t) == 8,
                   "idt_desc_t size is not 8 bytes");
KSTATIC_ASSERT_MSG(OFFSET_OF(struct idt_desc_t, offs_lb) == 0,
                   "idt_desc_t offs_lb offset is not 0");
KSTATIC_ASSERT_MSG(OFFSET_OF(struct idt_desc_t, sel) == 2,
                   "idt_desc_t sel offset is not 2");
KSTATIC_ASSERT_MSG(OFFSET_OF(struct idt_desc_t, nargs) == 4,
                   "idt_desc_t nargs offset is not 4");
KSTATIC_ASSERT_MSG(OFFSET_OF(struct idt_desc_t, attr) == 5,
                   "idt_desc_t attr offset is not 5");
KSTATIC_ASSERT_MSG(OFFSET_OF(struct idt_desc_t, offs_hb) == 6,
                   "idt_desc_t offs_hb offset is not 6");

void
init_idt(void);

bool
get_intr_status(void);

bool
set_intr_status(bool status);
