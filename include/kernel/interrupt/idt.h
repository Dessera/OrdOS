#pragma once

#define IDT_TYPE_INTR 0xe
#define IDT_TYPE_TRAP 0x6

#define IDT_DPL_KERNEL 0
#define IDT_DPL_USER 3

#define IDT_DESC_ATTR(p, dpl, type) ((p << 7) | (dpl << 5) | (type))

void
init_idt(void);
