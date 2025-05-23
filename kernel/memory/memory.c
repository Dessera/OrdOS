#include "kernel/memory/memory.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/sslab/sslab.h"
#include "kernel/memory/vpage.h"

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

void*
kmalloc(size_t size)
{
  return sslab_global_alloc(size);
}

void
kfree(void* obj)
{
  return sslab_global_free(obj);
}