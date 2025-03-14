#include "kernel/interrupt/idt.h"
#include "kernel/config/interrupt.h"
#include "kernel/memory/gdt.h"
#include "kernel/utils/asm.h"
#include "lib/types.h"

#define MK_IDT_PTR(idt_addr)                                                   \
  (((u64)(u32)idt_addr << 16) | (sizeof(idt_addr) - 1))

struct idt_desc_t
{
  u16 offs_lb;
  u16 sel;
  u8 nargs;
  u8 attr;
  u16 offs_hb;
} __attribute((packed));

extern void* _asm_intr_vecs[INTR_IDT_SIZE];

static struct idt_desc_t idt[INTR_IDT_SIZE];

static void
__idt_desc_init(struct idt_desc_t* idt_desc, u16 sel, u32 offs, u8 attr)
{
  idt_desc->offs_hb = (offs >> 16) & 0xffff;
  idt_desc->offs_lb = offs & 0xffff;
  idt_desc->nargs = 0;
  idt_desc->attr = attr;
  idt_desc->sel = sel;
}

static void
__init_idt_desc(void)
{
  for (u16 i = 0; i < INTR_IDT_SIZE; i++) {
    __idt_desc_init(&idt[i],
                    GDT_KCODE_SELECTOR,
                    (u32)_asm_intr_vecs[i],
                    IDT_DESC_ATTR(1, IDT_DPL_KERNEL, IDT_TYPE_INTR));
  }
  __idt_desc_init(&idt[0x80],
                  GDT_KCODE_SELECTOR,
                  (u32)_asm_intr_vecs[0x80],
                  IDT_DESC_ATTR(1, IDT_DPL_USER, IDT_TYPE_INTR));
}

void
init_idt(void)
{
  __init_idt_desc();

  u64 idt_ptr = MK_IDT_PTR(idt);
  lidt(idt_ptr);
}
