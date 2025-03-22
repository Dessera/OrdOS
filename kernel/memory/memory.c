#include "kernel/memory/memory.h"

#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/sslab/sslab.h"
#include "kernel/memory/vpage.h"
#include "lib/types.h"

void
init_memory(void)
{
  KINFO("initializing memory management subsystem");

  // initialize boot memory allocator
  init_bootmem();

  // initialize page table
  init_vpage();

  // initialize buddy
  init_buddy();

  // initialize sslab
  init_sslab();
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

FORCE_INLINE void*
kmalloc(size_t size)
{
  return sslab_global_alloc(size);
}

FORCE_INLINE void
kfree(void* obj)
{
  return sslab_global_free(obj);
}