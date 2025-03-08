#include "kernel/memory/memory.h"
#include "kernel/memory/page.h"
#include "kernel/utils/print.h"

void
init_mm(void)
{
  kputs("Initializing memory management...\n");

  init_page();
}