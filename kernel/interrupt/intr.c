#include "kernel/interrupt/intr.h"
#include "kernel/assert.h"
#include "kernel/config/interrupt.h"
#include "kernel/device/pci.h"
#include "kernel/interrupt/idt.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/print.h"

#define INTR_STATUS_MASK 0x200

static interrupt_handler_t interrupt_handlers[INTR_IDT_SIZE] = { 0 };

void
intr_register_handler(u32 interrupt_number, interrupt_handler_t handler)
{
  KASSERT_MSG(interrupt_number < INTR_IDT_SIZE, "Invalid interrupt ID");
  KWARNON_MSG(interrupt_handlers[interrupt_number],
              "Overwriting existing interrupt handler");
  interrupt_handlers[interrupt_number] = handler;
}

void
init_intr(void)
{
  kputs("Initializing Interrupt...\n");
  init_idt();
  init_pci();

  intr_set_status(true);
}

bool
intr_get_status(void)
{
  return (eflags() & INTR_STATUS_MASK) ? true : false;
}

bool
intr_set_status(bool status)
{
  bool old_status = intr_get_status();

  if (status && !old_status) {
    sti();
  } else if (!status && old_status) {
    cli();
  }

  return old_status;
}

void
intr_common_handler(u32 irq)
{
  KASSERT_MSG(irq < INTR_IDT_SIZE, "Invalid interrupt ID");

  if (interrupt_handlers[irq]) {
    interrupt_handlers[irq](irq);
  }
}