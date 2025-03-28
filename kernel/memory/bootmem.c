#include "kernel/memory/bootmem.h"
#include "kernel/assert.h"
#include "kernel/config/boot.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/compiler.h"
#include "kernel/utils/string.h"
#include "lib/common.h"
#include "lib/types.h"

uintptr_t __free_mem;
uintptr_t __mem_end;

void
init_bootmem(void)
{
  // memory below kernel is not used
  AUTO mem_start = MEM_KERNEL_PADDR(compiler_get_kernel_end());
  AUTO mem_end = e820_get_memory_size();

  __free_mem = mem_start;
  __mem_end = mem_end;

  KDEBUG("bootmem: %x - %x", mem_start, mem_end);
}

void*
bootmem_alloc(size_t size)
{
  uintptr_t base = ALIGN_UP(__free_mem, size);
  __free_mem = base + size;

  if (__free_mem > __mem_end) {
    KPANIC("bootmem is exhausted, alloc %x bytes failed", size);
  }

  void* ptr = (void*)MEM_KERNEL_VADDR(base);
  kmemset(ptr, 0, size);

  return ptr;
}

void
bootmem_pre_init_pages(struct page* pages, size_t page_cnt)
{
  KASSERT(pages != nullptr, "invalid pages when reserving bootmem pages");
  for (uintptr_t addr = BOOT_KERNEL_START; addr < __free_mem;
       addr += MEM_PAGE_SIZE) {
    size_t pg_idx = addr / MEM_PAGE_SIZE;
    if (pg_idx >= page_cnt) {
      break;
    }
    pages[pg_idx].reserved = true;
  }
}