#include "kernel/interrupt/idt.h"
#include "kernel/config/interrupt.h"
#include "kernel/device/pci.h"
#include "kernel/memory/gdt.h"
#include "kernel/types.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/print.h"

#define MK_IDT_PTR(idt_addr)                                                   \
  (((u64)(u32)idt_addr << 16) | (sizeof(idt_addr) - 1))

#define INTR_STATUS_MASK 0x200

void
intr_common_handler(u32 irq);

extern void* _asm_intr_vecs[INTR_IDT_SIZE];

static struct idt_desc_t idt[INTR_IDT_SIZE];

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
  for (u16 i = 0; i < INTR_IDT_SIZE; i++) {
    mkidt_desc(&idt[i],
               GDT_CODE_SELECTOR,
               (u32)_asm_intr_vecs[i],
               IDT_DESC_ATTR(1, IDT_DPL_KERNEL, IDT_TYPE_INTR));
  }
}

void
init_idt(void)
{
  kputs("Initializing IDT...\n");

  init_idt_desc();
  init_pci();

  u64 idt_ptr = MK_IDT_PTR(idt);
  lidt(idt_ptr);

  set_intr_status(true);
}

void
intr_common_handler(u32 irq)
{
  (void)irq;
}

bool
get_intr_status(void)
{
  return (eflags() & INTR_STATUS_MASK) ? true : false;
}

bool
set_intr_status(bool status)
{
  bool old_status = get_intr_status();

  if (status && !old_status) {
    sti();
  } else if (!status && old_status) {
    cli();
  }

  return old_status;
}