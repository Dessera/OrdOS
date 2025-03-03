#include "kernel/assert.h"
#include "kernel/interrupt/idt.h"
#include "kernel/stddef.h"
#include "kernel/utils/print.h"
void
kernel_panic_handler(u32 line,
                     const char* file,
                     const char* func,
                     const char* msg)
{
  set_intr_status(false);

  kputs("!! KERNEL PANIC !!\n");

  kputs("file: ");
  kputs(file);
  kputs("\n");
  ;

  kputs("line: ");
  kput_u32(line);
  kputs("\n");

  kputs("func: ");
  kputs(func);
  kputs("\n");

  kputs("msg: ");
  kputs(msg);
  kputs("\n");

  while (true)
    ;
}