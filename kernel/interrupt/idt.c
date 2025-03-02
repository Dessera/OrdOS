#include "kernel/interrupt/idt.h"
#include "kernel/mem.h"
#include "kernel/types.h"

extern void* _asm_intr_vecs[IDT_SIZE];

static struct idt_desc_t idt[IDT_SIZE];

static void
mkidt_desc(struct idt_desc_t* idt_desc, u16 sel, u32 offs, u8 attr)
{
  idt_desc->offs_hb = (offs >> 16) & 0xffff;
  idt_desc->offs_lb = offs & 0xffff;
  idt_desc->nargs = 0;
  idt_desc->attr = attr;
  idt_desc->sel = sel;
}

static void
init_idt_desc(void)
{
  for (u16 i = 0; i < IDT_SIZE; i++) {
    mkidt_desc(&idt[i],
               GDT_CODE_SELECTOR,
               (u32)_asm_intr_vecs[i],
               IDT_DESC_ATTR(1, IDT_DPL_KERNEL, IDT_TYPE_INTR));
  }
}

void
init_idt(void)
{
  init_idt_desc();
}