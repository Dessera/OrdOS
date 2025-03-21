
#include "kernel/interrupt/exception.h"
#include "kernel/assert.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "lib/asm.h"
#include "lib/types.h"

#define INTR_RESERVE_SIZE 20

static char* __exception_label[] = {
  "Divide Error",
  "Debug",
  "Non-Maskable Interrupt",
  "Breakpoint",
  "Overflow",
  "Bound Range Exceeded",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment Not Present",
  "Stack-Segment Fault",
  "General Protection Fault",
  "Page Fault",
  "Reserved",
  "x87 Floating-Point Exception",
  "Alignment Check",
  "Machine Check",
  "SIMD Floating-Point Exception",
};

KSTATIC_ASSERT(ARRAY_SIZE(__exception_label) == INTR_RESERVE_SIZE,
               "mismatched exception label size");

static void
__exception_handler(u32 intr)
{
  KPANIC("kernel panic because of %s !", __exception_label[intr]);
}

static void
__page_fault_handler(u32 intr)
{
  KPANIC("kernel panic because of %s, error addr: 0x%x !",
         __exception_label[intr],
         rcr2());
}

void
init_exception(void)
{
  for (int i = 0; i < INTR_RESERVE_SIZE; i++) {
    if (i == 14) { // page fault
      intr_register_handler(i, __page_fault_handler);
    } else {
      intr_register_handler(i, __exception_handler);
    }
  }

  KDEBUG("exceptions: %u", INTR_RESERVE_SIZE);
}