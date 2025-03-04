#pragma once

#include "kernel/boot.h"
#include "kernel/config/memory.h"

#define PAGE_INITIAL_ENTRIES ((PAGE_SIZE - PDE_KERNEL_OFFSET) / PDE_DESC_SIZE)

#define PDE_DESC_SIZE 4
#define PTE_DESC_SIZE PDE_DESC_SIZE

#define PDE_P(sign) (sign)
#define PDE_RW(sign) (sign << 1)
#define PDE_US(sign) (sign << 2)

#define PDE_DESC(base, p, w, u) ((base) | PDE_P(p) | PDE_RW(w) | PDE_US(u))
#define PTE_DESC(base, p, w, u) PDE_DESC(base, p, w, u)

#define INDEX_PAGE_TABLE_ADDR(index) ((index) * PAGE_SIZE + PAGE_TABLE_ADDR)

#define MEMORY_KB(kb) ((kb) * 1024)
#define MEMORY_MB(mb) (MEMORY_KB(mb) * 1024)

#define KERNEL_HEAP_START (KERNELBASE + MEMORY_MB(1))

#ifndef __ASM__

#include "kernel/stddef.h"
#include "kernel/utils/bitmap.h"

struct vmem_map
{
  struct bitmap vmmap;
  u32 vstart;
};

void
init_mm(void);

#endif
