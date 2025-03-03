#include "kernel/device/vga.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/print.h"

void
kmain(void);

extern void
ktester(void);

static void
kinit(void)
{
  init_vga();
}

void
kmain(void)
{
  kinit();

  kputs("Running tests...\n");

  ktester();

  kputs("All tests passed!");

  while (true) {
    hlt();
  }
}