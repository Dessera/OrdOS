#pragma once

#include "kernel/config/memory.h"

#define PAGE_DESC_SIZE 4

#define PAGE_ENTRIES (MEM_PAGE_SIZE / PAGE_DESC_SIZE)

#define PAGE_SELECTOR_MASK 0xfffff000

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

#define PAGE_PDE_INDEX(addr) (((addr) & PAGE_PDE_MASK) >> 22)
#define PAGE_PTE_INDEX(addr) (((addr) & PAGE_PTE_MASK) >> 12)

#define PAGE_PDE_KERNEL_OFFSET PAGE_PDE_INDEX(MEM_KERNEL_VSTART)

/**
 * @brief Initialize the page table and page directory
 */
void
init_page(void);