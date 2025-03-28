#include "kernel/memory/buddy/page.h"
#include "kernel/assert.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "lib/common.h" // IWYU pragma: keep
#include "lib/types.h"

static struct page* __pages;

void
init_page(void)
{
  size_t mem_pages = e820_get_pages_cnt();
  __pages = bootmem_alloc((sizeof(struct page) * mem_pages));

  e820_pre_init_pages(__pages, mem_pages);
  bootmem_pre_init_pages(__pages, mem_pages);

  KDEBUG("physical memory: %uMB, %u pages",
         mem_pages * MEMKB(4) / MEMMB(1),
         mem_pages);
}

FORCE_INLINE size_t
page_get_index(struct page* page)
{
  size_t idx = page - __pages;
  KASSERT(idx < e820_get_pages_cnt(), "page index out of bounds");
  return idx;
}

bool
page_index_is_overflow(size_t index)
{
  return index >= e820_get_pages_cnt();
}

FORCE_INLINE struct page*
page_get(size_t index)
{
  KASSERT(index < e820_get_pages_cnt(),
          "page index out of bounds: received: %u, max: %u",
          index,
          e820_get_pages_cnt());
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