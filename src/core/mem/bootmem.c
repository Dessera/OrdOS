#include "ordos/core/mem/bootmem.h"
#include "ordos/init.h"
#include "ordos/lib/common.h"
#include "ordos/lib/compiler.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/section.h"
#include "ordos/lib/types.h"

__prelude_data static uintptr_t __bootmem_start = 0;

__prelude_data static uintptr_t __bootmem_end = 0;

void
bootmem_init(void)
{
  struct init_info* init = vaccess(&__init);

  __bootmem_start = compiler_kernel_end_paddr();

  for (size_t i = 0; i < init->mmap_cnt; ++i) {
    if (init->mmap[i].type != PMEM_AVAILABLE) {
      continue;
    }

    u64 memsize = init->mmap[i].addr + init->mmap[i].len;
    if (memsize > __bootmem_end) {
      __bootmem_end = memsize;
    }
  }
}

void*
bootmem_alloc(size_t size)
{
  uintptr_t base = align_up(__bootmem_start, size);
  if (base + size > __bootmem_end) {
    kpanic_prelude("Prelude failed: boot memory overflow");
  }

  __bootmem_start = base + size;

  return (void*)base;
}

uintptr_t
bootmem_start(void)
{
  return __bootmem_start;
}

uintptr_t
bootmem_end(void)
{
  return __bootmem_end;
}
