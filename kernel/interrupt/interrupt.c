#include "kernel/config/interrupt.h"
#include "kernel/assert.h"
#include "kernel/device/pci.h"
#include "kernel/interrupt/exception.h"
#include "kernel/interrupt/idt.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/interrupt/syscall.h"
#include "kernel/log.h"
#include "lib/asm.h"

#define INTR_STATUS_MASK 0x200

static interrupt_handler_t __interrupt_handlers[INTR_IDT_SIZE] = { 0 };

void
intr_register_handler(u32 interrupt_number, interrupt_handler_t handler)
{
  KASSERT(interrupt_number < INTR_IDT_SIZE,
          "invalid interrupt id, received %x but max is %x",
          interrupt_number,
          INTR_IDT_SIZE);
  if (__interrupt_handlers[interrupt_number] != NULL) {
    KWARNING("overwriting existing interrupt handler at %x", interrupt_number);
  }
  __interrupt_handlers[interrupt_number] = handler;
}

void
init_intr(void)
{
  KINFO("initializing interrupt subsystem");

  // load the idt
  init_idt();

  // register exceptions
  init_exception();

  // register syscall handler
  init_syscall();

  // enable pic
  // TODO: Should not be here because it is a device
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

  if (__interrupt_handlers[irq] != NULL) {
    __interrupt_handlers[irq](irq);
  } else {
    KWARNING_NINT("unhandled interrupt %x occurred", irq);
  }
}