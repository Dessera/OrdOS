#include "ordos/kernel/mem/buddy/page.h"
#include "ordos/kernel/compiler.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/init.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem.h"
#include "ordos/kernel/mem/bootmem.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

struct page* __pages = NULL;

size_t __pages_cnt = 0;

static void
__mmap_reserve(void)
{
  for (size_t i = 0; i < __init.mmap_cnt; ++i) {
    if (__init.mmap[i].type == PMEM_AVAILABLE) {
      continue;
    }

    for (uintptr_t addr = __init.mmap[i].addr;
         addr < __init.mmap[i].addr + __init.mmap[i].len;
         addr += ORDOS_KERNEL_PAGE_SIZE) {
      size_t pg_idx = addr / ORDOS_KERNEL_PAGE_SIZE;
      if (pg_idx >= __pages_cnt) {
        break;
      }

      __pages[pg_idx].reserved = true;
    }
  }
}

static void
__bootmem_reserve(void)
{
  for (uintptr_t addr = compiler_kernel_start_paddr(); addr < bootmem_start();
       addr += ORDOS_KERNEL_PAGE_SIZE) {
    size_t pg_idx = addr / ORDOS_KERNEL_PAGE_SIZE;
    if (pg_idx >= __pages_cnt) {
      break;
    }
    __pages[pg_idx].reserved = true;
  }
}

void
init_page(void)
{
  __pages_cnt = div_down(bootmem_end(), ORDOS_KERNEL_PAGE_SIZE);
  __pages = bootmem_alloc((sizeof(struct page) * __pages_cnt));

  __mmap_reserve();
  __bootmem_reserve();

  kinfo("Physical memory %u mb, %u pages",
        page_size(__pages_cnt, MBYTES),
        __pages_cnt);
}
