#include "kernel/memory/page.h"
#include "kernel/config/memory.h"
#include "kernel/memory/bootmem.h"

void
init_page(void)
{
  // void* pde_entry = bootmem_alloc(MEM_PAGE_SIZE);
  // void* pte_entry =
  //   bootmem_alloc(MEM_PAGE_SIZE * PAGE_PDE_KERNEL_OFFSET /
  //   PAGE_PDE_DESC_SIZE);
}