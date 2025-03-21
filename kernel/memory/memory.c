#include "kernel/memory/memory.h"

#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/buddy.h"
#include "kernel/memory/page.h"

void
init_memory(void)
{
  KINFO("initializing memory management subsystem");

  // initialize boot memory allocator
  init_bootmem();

  // initialize page table
  init_paging();

  // initialize buddy
  init_buddy();
}

char*
mem_type_to_string(enum mem_type type)
{
  switch (type) {
    case MEM_ZONE_DMA:
      return "DMA";
    case MEM_ZONE_NORMAL:
      return "NORMAL";
    case MEM_ZONE_HIGH:
      return "HIGHMEM";
    default:
      return "UNKNOWN";
  }
}