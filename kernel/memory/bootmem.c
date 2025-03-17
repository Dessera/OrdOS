#include "kernel/memory/bootmem.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/compiler.h"
#include "lib/common.h"
#include "lib/types.h"

extern u16 _asm_mem_nr;
extern struct e820_entry _asm_mem_zone;

struct bootmem_allocator
{
  u32 start;
  u32 free;
  u32 end;
};

static struct bootmem_allocator __bootmem;

static FORCE_INLINE uintptr_t
__get_mem_end(struct e820_entry* mem_zone, u16 mem_nr)
{
  uintptr_t mem_end = 0;
  for (u16 i = 0; i < mem_nr; i++) {
    KTRACE("physical memory: <%x - %x> %s",
           (uintptr_t)mem_zone[i].base,
           (uintptr_t)(mem_zone[i].length + mem_zone[i].base - 1),
           e820_type_to_string(mem_zone[i].type));
    if (mem_zone[i].type != E820_TYPE_RAM) {
      continue;
    }

    if (mem_zone[i].base + mem_zone[i].length > mem_end) {
      mem_end = mem_zone[i].base + mem_zone[i].length;
    }
  }

  return mem_end;
}

void
init_bootmem(void)
{
  // e820 info
  AUTO mem_nr = *MEM_GET_WITH_KERNEL_VSTART(&_asm_mem_nr);
  AUTO mem_zone = MEM_GET_WITH_KERNEL_VSTART(&_asm_mem_zone);

  // memory below kernel is not used
  AUTO mem_start = MEM_KERNEL_PADDR(compiler_get_kernel_end());

  uintptr_t mem_end = __get_mem_end(mem_zone, mem_nr);

  KDEBUG("bootmem start: %x", mem_start);
  KDEBUG("bootmem end: %x", mem_end);

  __bootmem.start = mem_start;
  __bootmem.free = mem_start;
  __bootmem.end = mem_end;
}

void*
bootmem_alloc(u32 size)
{
  KASSERT(__bootmem.free != 0, "bootmem is not initialized");

  u32 base = ALIGN_UP(__bootmem.free, size);
  __bootmem.free = base + size;

  if (__bootmem.free > __bootmem.end) {
    KPANIC("bootmem is exhausted, alloc %x bytes failed", size);
  }

  return (void*)MEM_KERNEL_VADDR(base);
}

void*
bootmem_exit(void)
{
  u32 base = ALIGN_UP(__bootmem.free, MEM_PAGE_SIZE);
  return (void*)base;
}

size_t
bootmem_get_all_pages(void)
{
  return __bootmem.end / MEM_PAGE_SIZE;
}