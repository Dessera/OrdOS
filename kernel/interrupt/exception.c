
#include "kernel/interrupt/exception.h"
#include "kernel/assert.h"
#include "kernel/config/interrupt.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "lib/asm.h"
#include "lib/types.h"

KSTATIC_ASSERT(INTR_EXCEPTION_SIZE >= ENUM_SIZE(intr_exception),
               "exception size overflow");

static void
__exception_handler(u32 intr)
{
  KPANIC("kernel panic because of %s !", intr_exception_to_str(intr));
}

static void
__page_fault_handler(u32 intr)
{
  KPANIC("kernel panic because of %s, error addr: 0x%x !",
         intr_exception_to_str(intr),
         rcr2());
}

void
init_exception(void)
{
  for (size_t i = 0; i < ENUM_SIZE(intr_exception); i++) {
    if (i == INTR_TYPE_PAGE_FAULT) { // page fault
      intr_register_handler(i, __page_fault_handler);
    } else {
      intr_register_handler(i, __exception_handler);
    }
  }

  KDEBUG("exceptions: %u", ENUM_SIZE(intr_exception));
}
