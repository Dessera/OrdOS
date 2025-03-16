#include "kernel/interrupt/interrupt.h"
#include "lib/asm.h"

#define INTR_STATUS_MASK 0x200

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