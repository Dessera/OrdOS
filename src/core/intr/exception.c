#include "ordos/core/intr/exception.h"
#include "ordos/config.h"
#include "ordos/core/intr.h"
#include "ordos/lib/logging.h"

static const char* __intr_exception_labels[ORDOS_INTR_EXCEPTIONS_CNT] = {
  "IE_ZERO_DIVIDE",
  "IE_DEBUG",
  "IE_NMI",
  "IE_BREAKPOINT",
  "IE_OVERFLOW",
  "IE_BOUND_RANGE",
  "IE_INVALID_OPCODE",
  "IE_DEVICE_NOT_AVAILABLE",
  "IE_DOUBLE_FAULT",
  "IE_COSEGMENT_OVERLAP",
  "IE_INVALID_TSS",
  "IE_SEGMENT_NOT_PRESENT",
  "IE_STACK_FAULT",
  "IE_GENERAL_PROTECTION",
  "IE_PAGE_FAULT",
  "IE_RESERVED",
  "IE_X87_FLOATING_POINT_ERROR",
  "IE_ALIGNMENT_CHECK",
  "IE_MACHINE_CHECK",
  "IE_SIMD_FLOATING_POINT_ERROR"
};

static void
__exception_handler(enum intr_type intr)
{
  kpanic("Kernel panic because of %s !", __intr_exception_labels[intr]);
}

static void
__page_fault_handler(enum intr_type intr)
{
  kpanic("Kernel panic because of %s, error addr: 0x%x !",
         __intr_exception_labels[intr],
         rcr2());
}

void
init_exception(void)
{
  for (enum intr_type i = 0; i < ORDOS_INTR_EXCEPTIONS_CNT; i++) {
    if (i == IE_PAGE_FAULT) {
      intr_register(i, __page_fault_handler);
    } else {
      intr_register(i, __exception_handler);
    }
  }

  kdebug("Exceptions: %u handlers", ORDOS_INTR_EXCEPTIONS_CNT);
}
