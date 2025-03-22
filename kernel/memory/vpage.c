#include "kernel/memory/buddy/page.h"
#include "kernel/assert.h"
#include "kernel/config/boot.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/gdt.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/vpage.h"
#include "kernel/task/task.h"
#include "kernel/utils/compiler.h"
#include "kernel/utils/string.h"
#include "lib/asm.h"
#include "lib/common.h"
#include "lib/types.h"

extern struct gdt_ptr _asm_gdt_ptr;

static u32* __kernel_pde;

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
  return __kernel_pde;
}

FORCE_INLINE u32*
vpage_kernel_paddr(void)
{
  return (u32*)MEM_KERNEL_PADDR(__kernel_pde);
}

uintptr_t
vpage_link_addr(uintptr_t vaddr)
{
  AUTO curr = task_get_current();

  AUTO page_table = MEM_GET_WITH_KERNEL_VSTART(curr->page_table);
  KASSERT(page_table != NULL, "there is no page table in task %p", curr);

  AUTO ppage = buddy_alloc_page(MEM_ZONE_NORMAL, 0);
  if (ppage == NULL) {
    KWARNING("alloc page failed when link vaddr %p", vaddr);
    return 0;
  }

  AUTO pde = page_table[PAGE_PDE_INDEX(vaddr)];
  u32* pt = NULL;

  if (!(PAGE_P_MASK & pde)) {
    // alloc page table
    AUTO pt_page = buddy_alloc_page(MEM_ZONE_NORMAL, 0);
    if (pt_page == NULL) {
      KWARNING("alloc page table failed when link vaddr %p", vaddr);
      buddy_free_page(ppage, 0);
      return 0;
    }

    page_table[PAGE_PDE_INDEX(vaddr)] =
      PAGE_PDE_DESC(page_get_phys(pt_page), 1, 1, 1);
    pt = (u32*)page_get_virt(pt_page);
  } else {
    pt = (u32*)MEM_KERNEL_VADDR(PAGE_PADDR(pde));
  }

  AUTO pte = pt[PAGE_PTE_INDEX(vaddr)];
  KASSERT(!(PAGE_P_MASK & pte), "try to link vaddr %p twice", vaddr);

  pt[PAGE_PTE_INDEX(vaddr)] = PAGE_PTE_DESC(page_get_phys(ppage), 1, 1, 1);

  return vaddr;
}
