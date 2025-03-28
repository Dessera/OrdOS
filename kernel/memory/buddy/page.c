#include "kernel/memory/buddy/page.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "lib/common.h"
#include "lib/types.h"

static struct page* __pages;

void
init_page(void)
{
  uintptr_t mem_size = e820_get_memory_size();
  size_t mem_pages = DIV_DOWN(mem_size, MEM_PAGE_SIZE);
  __pages = bootmem_alloc((sizeof(struct page) * mem_pages));

  e820_pre_init_pages(__pages, mem_pages);
  bootmem_pre_init_pages(__pages, mem_pages);

  KDEBUG("physical memory: %uMB, %u pages", mem_size / MEMMB(1), mem_pages);
}

FORCE_INLINE size_t
page_get_index(struct page* page)
{
  return page - __pages;
}

FORCE_INLINE struct page*
page_get(size_t index)
{
  return &__pages[index];
}

FORCE_INLINE uintptr_t
page_get_phys(struct page* page)
{
  return page_get_index(page) * MEM_PAGE_SIZE;
}

FORCE_INLINE struct page*
page_get_by_phys(uintptr_t phys)
{
  return page_get(phys / MEM_PAGE_SIZE);
}

FORCE_INLINE uintptr_t
page_get_virt(struct page* page)
{
  return MEM_KERNEL_VADDR(page_get_phys(page));
}

FORCE_INLINE struct page*
page_get_by_virt(uintptr_t virt)
{
  return page_get_by_phys(MEM_KERNEL_PADDR(virt));
}