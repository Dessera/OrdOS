#include "kernel/interrupt/idt.h"
#include "kernel/config/interrupt.h"
#include "kernel/defs.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "kernel/memory/gdt.h"
#include "lib/asm.h"
#include "lib/types.h"

extern void* _asm_intr_vecs[INTR_IDT_SIZE];

static struct idt_desc __idt[INTR_IDT_SIZE];

static void
__idt_desc_init(struct idt_desc* desc,
                void* entry,
                u16 sel,
                enum idt_desc_type type,
                enum dpl dpl)
{
  desc->offs_hb = (u16)((u32)entry >> 16);
  desc->offs_lb = (u16)((u32)entry & 0xffff);
  desc->sel = sel;
  desc->attrs = (1 << 7) | (dpl << 5) | (type);
  desc->reserved = 0;
}

void
init_idt(void)
{
  // other interrupts
  for (u16 i = 0; i < INTR_IDT_SIZE; i++) {
    __idt_desc_init(&__idt[i],
                    _asm_intr_vecs[i],
                    GDT_KCODE_SELECTOR,
                    IDT_DESC_TYPE_INT,
                    DPL_KERNEL);
  }

  // syscall
  __idt_desc_init(&__idt[INTR_TYPE_SYSCALL],
                  _asm_intr_vecs[INTR_TYPE_SYSCALL],
                  GDT_KCODE_SELECTOR,
                  IDT_DESC_TYPE_INT,
                  DPL_USER);

  lidt(idt_get_ptr(__idt, ARRAY_SIZE(__idt)));

  KDEBUG("idt: %p", __idt);
}
