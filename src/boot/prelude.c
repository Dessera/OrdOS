#include "ordos/boot/multiboot.h"
#include "ordos/config.h"
#include "ordos/core/mem/bootmem.h"
#include "ordos/core/mem/buddy/zone.h"
#include "ordos/core/mem/gdt.h"
#include "ordos/core/mem/vpage.h"
#include "ordos/init.h"
#include "ordos/lib/common.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/section.h"
#include "ordos/lib/types.h"

/**
 * @brief Initialize memory map.
 *
 */
__prelude static void
__init_mmap(struct init_info* init, struct multiboot_mmap* mmap)
{
  for (struct multiboot_mmap_entry* entry = mmap->entries;
       (void*)entry < poffset(mmap, mmap->tag.size);
       entry = poffset(entry, mmap->entry_size)) {
    if (init->mmap_cnt >= ORDOS_INIT_MMAP_CNT) {
      break;
    }

    init->mmap[init->mmap_cnt].type = entry->type;
    init->mmap[init->mmap_cnt].addr = entry->addr;
    init->mmap[init->mmap_cnt].len = entry->len;
    ++init->mmap_cnt;
  }
}

/**
 * @brief Initialize command line.
 *
 */
__prelude static void
__init_cmdline(struct init_info* init, struct multiboot_cmdline* cmdline)
{
  char* src = cmdline->string;
  char* dest = init->cmdline;
  size_t size = ORDOS_INIT_ARGS_BUFSIZE;
  while (size-- && (*dest++ = *src++)) {
  }
}

/**
 * @brief Initialize from multiboot tags.
 *
 */
__prelude static void
__init_from_multiboot(struct init_info* init, void* tags)
{
  struct multiboot_mmap* mmap = NULL;
  struct multiboot_cmdline* cmdline = NULL;

  for (struct multiboot_tag* tag = tags; tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = poffset(tag, align_up(tag->size, MULTIBOOT_TAG_ALIGN))) {
    if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
      mmap = (struct multiboot_mmap*)tag;
    }

    if (tag->type == MULTIBOOT_TAG_TYPE_CMDLINE) {
      cmdline = (struct multiboot_cmdline*)tag;
    }
  }

  if (mmap != NULL) {
    __init_mmap(init, mmap);
  } else {
    kpanic_prelude("Prelude failed: Cannot fetch memory map");
  }

  if (cmdline != NULL) {
    __init_cmdline(init, cmdline);
  }
}

/**
 * @brief Load initial page table.
 *
 */
__prelude static void
__init_pagetable(struct init_info* init)
{
  pde_t* pd = bootmem_alloc(ORDOS_KERNEL_PAGE_SIZE);
  size_t pde_cnt =
    min(div_up(bootmem_end() / ORDOS_KERNEL_PAGE_SIZE, VPAGE_DESC_CNT),
        pde_index(MEM_TYPE_HIGH_START));
  pte_t* pt = bootmem_alloc(pde_cnt * ORDOS_KERNEL_PAGE_SIZE);
  size_t pte_cnt = pde_cnt * VPAGE_DESC_CNT;

  void* pt_iter = pt;
  for (size_t i = 0; i < pde_cnt; ++i) {
    pd[i] = pde_desc(pt_iter, PG_PRESENT | PG_WRITABLE | PG_KERNEL);
    pd[i + pde_index(ORDOS_KERNEL_VADDR)] = pd[i];
    pt_iter = poffset(pt_iter, ORDOS_KERNEL_PAGE_SIZE);
  }

  for (size_t i = 0; i < pte_cnt; ++i) {
    pt[i] = pte_desc(i * ORDOS_KERNEL_PAGE_SIZE,
                     PG_PRESENT | PG_WRITABLE | PG_KERNEL);
  }

  vpage_load(pd);
  vpage_enable();

  init->kernel_pd = pd;
  init->kernel_pde_cnt = pde_cnt;
  init->kernel_pt = pt;
  init->kernel_pte_cnt = pte_cnt;
}

/**
 * @brief Initialize prelude memory.
 *
 */
__prelude static void
__init_mem(struct init_info* init)
{
  bootmem_init();
  __init_pagetable(init);

  gdtr_t gdtr = gdt_create_ptr(__gdt, ORDOS_MEM_GDT_DESC_CNT);
  asm_exec("lgdt %0" : : "m"(gdtr));
  asm_exec("ljmp %0, $1f;"
           "1:" : : "i"(gdt_sel_kcode()));
  asm_exec("movw %%ax, %%ds;"
           "movw %%ax, %%es;"
           "movw %%ax, %%fs;"
           "movw %%ax, %%gs;"
           "movw %%ax, %%ss;" : : "a"(gdt_sel_kdata()));
}

/**
 * @brief Kernel prelude.
 *
 */
__asm_linkage __prelude void
kprelude(u32 magic, void* info)
{
  struct init_info* init = vaccess(&__init);

  if (unlikely(magic != MULTIBOOT_LOADER_MAGIC)) {
    kpanic_prelude("Prelude failed: Mismatched multiboot magic");
  }

  __init_from_multiboot(init,
                        poffset(info, sizeof(struct multiboot_info_header)));

  __init_mem(init);
}
