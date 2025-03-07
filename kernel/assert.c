#include "kernel/assert.h"
#include "kernel/interrupt/intr.h"
#include "kernel/types.h"
#include "kernel/utils/print.h"
void
kernel_panic_handler(u32 line,
                     const char* file,
                     const char* func,
                     const char* msg)
{
  intr_set_status(false);

  kputs("[KERNEL_PANIC] ");
  kputs(msg);
  kputs(" at ");
  kputs(file);
  kputs(":");
  kput_u32(line);
  kputs(" in ");
  kputs(func);
  kputs("\n");

  while (true)
    ;
}

void
kernel_warn_handler(u32 line,
                    const char* file,
                    const char* func,
                    const char* msg)
{
  kputs("[KERNEL_WARNING] ");
  kputs(msg);
  kputs(" at ");
  kputs(file);
  kputs(":");
  kput_u32(line);
  kputs(" in ");
  kputs(func);
  kputs("\n");
}