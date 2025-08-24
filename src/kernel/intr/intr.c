#include "ordos/kernel/intr/intr.h"
#include "ordos/kernel/assert.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/driver/pic.h"
#include "ordos/kernel/intr/exception.h"
#include "ordos/kernel/intr/idt.h"
#include "ordos/kernel/intr/syscall.h"
#include "ordos/kernel/logging.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

static intr_handler_t __intr_handlers[ORDOS_INTR_IDT_DESC_CNT] = { 0 };

void
init_intr(void)
{
  kinfo("Initializing interrupt subsystem");

  init_idt();
  init_exception();
  init_syscall();
  init_pic();
}

void
intr_register(intr_code_t code, intr_handler_t handler)
{
  kassert(code < ORDOS_INTR_IDT_DESC_CNT,
          "Interrupt: Invalid interrupt id, received %x but max is %x",
          code,
          ORDOS_INTR_IDT_DESC_CNT);

  bool status = intr_set_status(false);

  if (__intr_handlers[code] != NULL) {
    kwarn("Interrupt: Overwriting existing interrupt handler at %x", code);
  }
  __intr_handlers[code] = handler;

  intr_set_status(status);
}

void
intr_unregister(intr_code_t code)
{
  kassert(code < ORDOS_INTR_IDT_DESC_CNT,
          "Interrupt: Invalid interrupt id, received %x but max is %x",
          code,
          ORDOS_INTR_IDT_DESC_CNT);

  bool status = intr_set_status(false);

  if (__intr_handlers[code] != NULL) {
    __intr_handlers[code] = NULL;
  }

  intr_set_status(status);
}

bool
intr_get_status(void)
{
  return eflags() & INTR_EFLAG;
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

__asm_linkage void
intr_common_handler(u32 irq)
{
  kassert(irq < ORDOS_INTR_IDT_DESC_CNT,
          "Interrupt: Invalid interrupt id, received %x but max is %x",
          irq,
          ORDOS_INTR_IDT_DESC_CNT);

  if (__intr_handlers[irq] != NULL) {
    __intr_handlers[irq](irq);
  } else {
    kwarn_unsafe("Interrupt: Unhandled interrupt %x", irq);
  }
}
