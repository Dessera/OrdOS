#include "kernel/memory/vpage.h"
#include "kernel/assert.h"
#include "kernel/config/boot.h"
#include "kernel/config/memory.h"
#include "kernel/defs.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/gdt.h"
#include "kernel/memory/memory.h"
#include "kernel/task/task.h"
#include "kernel/utils/compiler.h"
#include "kernel/utils/string.h"
#include "lib/asm.h"
#include "lib/common.h"
#include "lib/types.h"

static u32* __kernel_pd;

static void
__link_top_kernel_mem(void)
{
  AUTO kernel_end = MEM_KERNEL_PADDR(compiler_get_kernel_end());

  // low 4mb
  __kernel_pd[0] = __kernel_pd[PAGE_PDE_KERNEL_OFFSET];

  // all kernel mem
  for (size_t i = PAGE_PDE_INDEX(BOOT_KERNEL_START);
       i <= PAGE_PDE_INDEX(kernel_end);
       i++) {
    __kernel_pd[i] = __kernel_pd[PAGE_PDE_KERNEL_OFFSET + i];
  }
}

static void
__unlink_top_kernel_mem(void)
{
  AUTO kernel_end = MEM_KERNEL_PADDR(compiler_get_kernel_end());

  // low 4mb
  __kernel_pd[0] = 0;

  // all kernel mem
  for (size_t i = PAGE_PDE_INDEX(BOOT_KERNEL_START);
       i <= PAGE_PDE_INDEX(kernel_end);
       i++) {
    __kernel_pd[i] = 0;
  }
}

static void
__init_early_page(void)
{
  // Kernel page directory
  __kernel_pd = bootmem_alloc(MEM_PAGE_SIZE);

  AUTO all_pages = e820_get_pages_cnt();
  AUTO kernel_pde_cnt =
    MIN(DIV_UP(all_pages, PAGE_ENTRIES), PAGE_PDE_INDEX(MEM_TYPE_HIGH_START));

  // Kernel page table
  u32* kernel_pt = bootmem_alloc(kernel_pde_cnt * MEM_PAGE_SIZE);

  // Set up kernel page directory
  uintptr_t pte_it = (uintptr_t)kernel_pt;
  for (size_t i = 0; i < kernel_pde_cnt; i++) {
    __kernel_pd[i + PAGE_PDE_KERNEL_OFFSET] =
      PAGE_PDE_DESC(MEM_KERNEL_PADDR(pte_it), PRESENT, 1, 1);
    pte_it += MEM_PAGE_SIZE;
  }

  // Set up kernel page table
  for (size_t i = 0; i < kernel_pde_cnt * PAGE_ENTRIES; i++) {
    kernel_pt[i] = PAGE_PTE_DESC(i * MEM_PAGE_SIZE, PRESENT, 1, 1);
  }

  __link_top_kernel_mem();
  lcr3((void*)MEM_KERNEL_PADDR(__kernel_pd));
}

static void
__init_post_page(void)
{
  __unlink_top_kernel_mem();
  lcr3((void*)MEM_KERNEL_PADDR(__kernel_pd));
}

static void
__init_post_gdt(void)
{
  gdt[GDT_KCODE_INDEX] = GDT_DESC_KCODE();
  gdt[GDT_KDATA_INDEX] = GDT_DESC_KDATA();
  gdt[GDT_VIDEO_INDEX] = GDT_DESC_VIDEO();

  AUTO gdt_ptr = gdt_get_ptr(gdt, ARRAY_SIZE(gdt));
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

void
init_vpage(void)
{
  // page table with top mem map
  __init_early_page();

  lcr0(rcr0() | 0x80000000);

  // reset gdt offset
  __init_post_gdt();

  // page table without top mem map
  __init_post_page();
}

FORCE_INLINE u32*
vpage_kernel_vaddr(void)
{
  return __kernel_pd;
}

FORCE_INLINE uintptr_t
vpage_kernel_paddr(void)
{
  return MEM_KERNEL_PADDR(__kernel_pd);
}

void*
vpage_link_addr(uintptr_t vaddr)
{
  AUTO curr = task_get_current();
  if (curr == nullptr) {
    return nullptr;
  }

  AUTO page_table = (u32*)MEM_KERNEL_VADDR(curr->page_table);
  KASSERT(page_table != nullptr, "there is no page table in task %p", curr);

  AUTO ppage = buddy_alloc_page(MEM_ZONE_NORMAL, 0);
  if (ppage == nullptr) {
    KWARNING("alloc page failed when link vaddr %p", vaddr);
    return nullptr;
  }

  kmemset((void*)page_get_virt(ppage), 0, MEM_PAGE_SIZE);

  AUTO pde = page_table[PAGE_PDE_INDEX(vaddr)];
  u32* pt = nullptr;

  if (!(PRESENT & pde)) {
    // alloc page table
    AUTO pt_page = buddy_alloc_page(MEM_ZONE_NORMAL, 0);
    if (pt_page == nullptr) {
      KWARNING("alloc page table failed when link vaddr %p", vaddr);
      buddy_free_page(ppage, 0);
      return nullptr;
    }

    kmemset((void*)page_get_virt(pt_page), 0, MEM_PAGE_SIZE);

    page_table[PAGE_PDE_INDEX(vaddr)] =
      PAGE_PDE_DESC(page_get_phys(pt_page), PRESENT, 1, 1);
    pt = (u32*)page_get_virt(pt_page);
  } else {
    pt = (u32*)MEM_KERNEL_VADDR(PAGE_PADDR(pde));
  }

  KASSERT(!(PRESENT & pt[PAGE_PTE_INDEX(vaddr)]),
          "try to link vaddr %p twice",
          vaddr);

  pt[PAGE_PTE_INDEX(vaddr)] =
    PAGE_PTE_DESC(page_get_phys(ppage), PRESENT, 1, 1);

  return (void*)vaddr;
}
