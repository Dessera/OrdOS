#include "ordos/core/intr.h"
#include "ordos/config.h"
#include "ordos/core/intr/exception.h"
#include "ordos/core/intr/idt.h"
#include "ordos/core/intr/syscall.h"
#include "ordos/lib/assert.h"
#include "ordos/lib/common.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/types.h"
#include "ordos/module.h"

static intr_handler_t __intr_handlers[ORDOS_INTR_IDT_DESC_CNT] = { 0 };

void
intr_register(enum intr_type code, intr_handler_t handler)
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
intr_unregister(enum intr_type code)
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
  }
}

int
intr_entry(struct module* mod)
{
  (void)mod;

  init_idt();
  init_exception();
  init_syscall();

  return E_SUCCESS;
}

module_dependency(drv_pic);

module_init_noexit(sys_intr, MOD_CORE | MOD_AUTOLOAD, intr_entry, drv_pic);
