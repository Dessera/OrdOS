#include "kernel/memory/bootmem.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/compiler.h"
#include "lib/common.h"
#include "lib/types.h"

struct bootmem_allocator
{
  uintptr_t start;
  uintptr_t free;
  uintptr_t end;
};

static struct bootmem_allocator __bootmem;

void
init_bootmem(void)
{
  // memory below kernel is not used
  AUTO mem_start = MEM_KERNEL_PADDR(compiler_get_kernel_end());
  AUTO mem_end = e820_get_memory_size();

  __bootmem.start = mem_start;
  __bootmem.free = mem_start;
  __bootmem.end = mem_end;

  KDEBUG("bootmem start: %x", mem_start);
  KDEBUG("bootmem end: %x", mem_end);
}

void*
bootmem_alloc(size_t size)
{
  KASSERT(__bootmem.free != 0, "bootmem is not initialized");

  uintptr_t base = ALIGN_UP(__bootmem.free, size);
  __bootmem.free = base + size;

  if (__bootmem.free > __bootmem.end) {
    KPANIC("bootmem is exhausted, alloc %x bytes failed", size);
  }

  return (void*)MEM_KERNEL_VADDR(base);
}

size_t
bootmem_get_all_pages(void)
{
  return __bootmem.end / MEM_PAGE_SIZE;
}