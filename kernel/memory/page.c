#include "kernel/memory/page.h"
#include "kernel/config/boot.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/gdt.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/compiler.h"
#include "kernel/utils/string.h"
#include "lib/asm.h"
#include "lib/common.h"
#include "lib/types.h"

extern struct gdt_ptr _asm_gdt_ptr;

static u32* __kernel_pde;
static struct page* __pages;

static void
__link_top_kernel_mem(void)
{
  AUTO kernel_end = MEM_KERNEL_PADDR(compiler_get_kernel_end());

  // low 4mb
  __kernel_pde[0] = __kernel_pde[PAGE_PDE_KERNEL_OFFSET];

  // all kernel mem
  for (size_t i = PAGE_PDE_INDEX(BOOT_KERNEL_START);
       i <= PAGE_PDE_INDEX(kernel_end);
       i++) {
    __kernel_pde[i] = __kernel_pde[PAGE_PDE_KERNEL_OFFSET + i];
  }
}

static void
__unlink_top_kernel_mem(void)
{
  AUTO kernel_end = MEM_KERNEL_PADDR(compiler_get_kernel_end());

  // low 4mb
  __kernel_pde[0] = 0;

  // all kernel mem
  for (size_t i = PAGE_PDE_INDEX(BOOT_KERNEL_START);
       i <= PAGE_PDE_INDEX(kernel_end);
       i++) {
    __kernel_pde[i] = 0;
  }
}

static void
__init_early_page(void)
{
  // Kernel page directory
  __kernel_pde = bootmem_alloc(MEM_PAGE_SIZE);
  kmemset(__kernel_pde, 0, MEM_PAGE_SIZE);

  AUTO all_pages = e820_get_pages_cnt();
  AUTO kernel_pde_cnt =
    MIN(DIV_UP(all_pages, PAGE_ENTRIES), PAGE_PDE_INDEX(MEM_TYPE_HIGH_START));

  // Kernel page table
  u32* kernel_pte = bootmem_alloc(kernel_pde_cnt * MEM_PAGE_SIZE);

  // Set up kernel page directory
  uintptr_t pte_it = (uintptr_t)kernel_pte;
  for (size_t i = 0; i < kernel_pde_cnt; i++) {
    __kernel_pde[i + PAGE_PDE_KERNEL_OFFSET] =
      PAGE_PDE_DESC(MEM_KERNEL_PADDR(pte_it), 1, 1, 1);
    pte_it += MEM_PAGE_SIZE;
  }

  // Set up kernel page table
  for (size_t i = 0; i < kernel_pde_cnt * PAGE_ENTRIES; i++) {
    kernel_pte[i] = PAGE_PTE_DESC(i * MEM_PAGE_SIZE, 1, 1, 1);
  }

  __link_top_kernel_mem();
  lcr3((void*)MEM_KERNEL_PADDR(__kernel_pde));
}

static void
__init_post_page(void)
{
  __unlink_top_kernel_mem();
  lcr3((void*)MEM_KERNEL_PADDR(__kernel_pde));
}

static void
__init_post_gdt(void)
{
  gdt[GDT_KCODE_INDEX] = GDT_DESC_KCODE();
  gdt[GDT_KDATA_INDEX] = GDT_DESC_KDATA();
  gdt[GDT_VIDEO_INDEX] = GDT_DESC_VIDEO();

  u64 gdt_ptr = GDT_GET_PTR(MEM_GDT_SIZE, gdt);
  __asm__ __volatile__("lgdt %0" : : "m"(gdt_ptr));
  __asm__ __volatile__("movw %%ax, %%ds;"
                       "movw %%ax, %%es;"
                       "movw %%ax, %%fs;"
                       "movw %%ax, %%gs;"
                       "movw %%ax, %%ss;"
                       "ljmp %1, $1f;"
                       "1:"
                       :
                       : "a"(GDT_KDATA_SELECTOR), "i"(GDT_KCODE_SELECTOR));
}

static void
__init_pmem_pool(void)
{
  uintptr_t mem_size = e820_get_memory_size();
  size_t mem_pages = DIV_DOWN(mem_size, MEM_PAGE_SIZE);
  __pages = bootmem_alloc((sizeof(struct page) * mem_pages));
  kmemset(__pages, 0, sizeof(struct page) * mem_pages);

  e820_pre_init_pages(__pages, mem_pages);
  bootmem_pre_init_pages(__pages, mem_pages);

  KDEBUG("physical memory: %uMB, %u pages", mem_size / MEMMB(1), mem_pages);
}

void
init_page(void)
{
  // page table with top mem map
  __init_early_page();

  u32 cr0 = rcr0();
  cr0 |= 0x80000000;
  lcr0(cr0);

  // reset gdt offset
  __init_post_gdt();

  // page table without top mem map
  __init_post_page();

  // init pmem pool
  __init_pmem_pool();
}

size_t
page_get_index(struct page* page)
{
  return page - __pages;
}

struct page*
page_get(size_t index)
{
  return &__pages[index];
}

uintptr_t
page_get_phys(struct page* page)
{
  return page_get_index(page) * MEM_PAGE_SIZE;
}