#include "kernel/config/interrupt.h"
#include "kernel/assert.h"
#include "kernel/device/pci.h"
#include "kernel/interrupt/idt.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "kernel/utils/asm.h"

#define INTR_STATUS_MASK 0x200

static interrupt_handler_t interrupt_handlers[INTR_IDT_SIZE] = { 0 };

static void
__default_nop_handler(u32 irq)
{
  (void)irq;
}

static void
__default_exception_handler(u32 irq)
{
  KPANIC("unhandled exception %x occurred", irq);
}

static void
__init_exception_handlers(void)
{
  for (u32 i = 0; i < INTR_RESERVE_SIZE; i++) {
    switch (i) {
      case 0x0d:
        intr_register_handler(i, __default_nop_handler);
        break;
      default:
        intr_register_handler(i, __default_exception_handler);
    }
  }
}

void
intr_register_handler(u32 interrupt_number, interrupt_handler_t handler)
{
  KASSERT(interrupt_number < INTR_IDT_SIZE,
          "invalid interrupt id, received %x but max is %x",
          interrupt_number,
          INTR_IDT_SIZE);
  if (interrupt_handlers[interrupt_number] != NULL) {
    KWARNING("overwriting existing interrupt handler at %x", interrupt_number);
  }
  interrupt_handlers[interrupt_number] = handler;
}

void
init_intr(void)
{
  KINFO("initializing interrupt subsystem");
  init_idt();

  __init_exception_handlers();

  init_pci();
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
  KASSERT(irq < INTR_IDT_SIZE,
          "invalid interrupt id, received %x but max is %x",
          irq,
          INTR_IDT_SIZE);

  if (interrupt_handlers[irq] != NULL) {
    interrupt_handlers[irq](irq);
  } else {
    KWARNING("unhandled interrupt %x occurred", irq);
  }
}