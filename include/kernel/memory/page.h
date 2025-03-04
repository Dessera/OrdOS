#pragma once

#include "common.h"
#include "kernel/config/memory.h"

#define PAGE_PDE_DESC_SIZE 4
#define PAGE_PTE_DESC_SIZE PAGE_PDE_DESC_SIZE

#define PAGE_PDE_VSTART 0xfffff000
#define PAGE_PTE_VSTART 0xffc00000

#define PAGE_PDE_MASK 0xffc00000
#define PAGE_PTE_MASK 0x003ff000
#define PAGE_ADDR_MASK 0x00000fff

#define PAGE_PDE_P(sign) (sign)
#define PAGE_PDE_RW(sign) ((sign) << 1)
#define PAGE_PDE_US(sign) ((sign) << 2)

#define PAGE_PDE_P_MASK (PAGE_PDE_P(1))
#define PAGE_PTE_P_MASK PAGE_PDE_P_MASK

#define PAGE_PDE_DESC(base, p, w, u)                                           \
  ((base) | PAGE_PDE_P(p) | PAGE_PDE_RW(w) | PAGE_PDE_US(u))
#define PAGE_PTE_DESC(base, p, w, u) PAGE_PDE_DESC(base, p, w, u)

#define PAGE_TABLE_INDEX(index) ((index) * MEM_PAGE_SIZE + MEM_PAGE_TABLE_START)

#define PAGE_PDE_KERNEL_OFFSET (MEM_KERNEL_VSTART >> 20)

#define PAGE_PDE_INDEX(vaddr) (((u32)(vaddr) & PAGE_PDE_MASK) >> 22)
#define PAGE_PTE_INDEX(vaddr) (((u32)(vaddr) & PAGE_PTE_MASK) >> 12)

#define PAGE_GET_PDE(vaddr)                                                    \
  ((u32*)(PAGE_PDE_INDEX(vaddr) * 4 + PAGE_PDE_VSTART))
#define PAGE_GET_PTE(vaddr)                                                    \
  ((u32*)(PAGE_PTE_INDEX(vaddr) * 4 + PAGE_PTE_VSTART +                        \
          (((u32)vaddr & PAGE_PDE_MASK) >> 10)))

#define PAGE_VADDR_TO_PADDR(vaddr)                                             \
  ((void*)((*PAGE_GET_PTE(vaddr) & PAGE_PDE_VSTART) +                          \
           ((u32)vaddr & PAGE_ADDR_MASK)))

#define PAGE_INITIAL_ENTRIES                                                   \
  ((MEM_PAGE_SIZE - PAGE_PDE_KERNEL_OFFSET) / PAGE_PDE_DESC_SIZE)

#define PAGE_BITMAP_VSTART (MEM_KERNEL_VSTART + MEM_POOL_BITMAP_OFFSET)
#define PAGE_KERNEL_HEAP_VSTART (MEM_KERNEL_VSTART + KMEMMB(1))
#define PAGE_KERNEL_STACK_VSTART (MEM_KERNEL_VSTART + MEM_KERNEL_STACK_OFFSET)

#ifndef __ASM__

#include "kernel/types.h"
#include "kernel/utils/bitmap.h"

struct vmem_map
{
  struct bitmap vmmap;
  u32 vstart;
};

void
init_mm(void);

void*
alloc_page(size_t size);

void
free_page(void* addr, size_t size);

#endif
