#include "kernel/memory/memory.h"
#include "kernel/log.h"
#include "kernel/memory/page.h"
#include "kernel/utils/print.h"

void
init_mm(void)
{
  KINFO("initializing memory management subsystem");

  init_page();
}