#include "kernel/memory/memory.h"

#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/page.h"

void
init_memory(void)
{
  KINFO("initializing memory management subsystem");

  init_bootmem();
  init_page();
}