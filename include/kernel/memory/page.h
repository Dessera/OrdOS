#pragma once

#include "common.h"
#include "kernel/config/memory.h"

#define PAGE_PDE_DESC_SIZE 4
#define PAGE_PTE_DESC_SIZE PAGE_PDE_DESC_SIZE

#define PAGE_PDE_P(sign) (sign)
#define PAGE_PDE_RW(sign) ((sign) << 1)
#define PAGE_PDE_US(sign) ((sign) << 2)

#define PAGE_PDE_DESC(base, p, w, u)                                           \
  ((base) | PAGE_PDE_P(p) | PAGE_PDE_RW(w) | PAGE_PDE_US(u))
#define PAGE_PTE_DESC(base, p, w, u) PAGE_PDE_DESC(base, p, w, u)

#define PAGE_TABLE_INDEX(index) ((index) * MEM_PAGE_SIZE + MEM_PAGE_TABLE_START)

#define PAGE_PDE_KERNEL_OFFSET (MEM_KERNEL_VSTART >> 20)

#define PAGE_INITIAL_ENTRIES                                                   \
  ((MEM_PAGE_SIZE - PAGE_PDE_KERNEL_OFFSET) / PAGE_PDE_DESC_SIZE)

#define PAGE_BITMAP_VSTART (MEM_KERNEL_VSTART + MEM_POOL_BITMAP_OFFSET)
#define PAGE_KERNEL_HEAP_VSTART (MEM_KERNEL_VSTART + KMEMMB(1))
#define PAGE_KERNEL_STACK_VSTART (MEM_KERNEL_VSTART + MEM_KERNEL_STACK_OFFSET)

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
