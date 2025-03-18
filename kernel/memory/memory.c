#include "kernel/memory/memory.h"

#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/page.h"

void
init_memory(void)
{
  KINFO("initializing memory management subsystem");

  // initialize boot memory allocator
  init_bootmem();

  // initialize page table
  init_page();

  // initialize buddy
}