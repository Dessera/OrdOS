#pragma once

#include "kernel/config/memory.h"

#define PAGE_DESC_SIZE 4

#define PAGE_ENTRIES (MEM_PAGE_SIZE / PAGE_DESC_SIZE)

#define PAGE_PDE_P(sign) (sign)
#define PAGE_PDE_RW(sign) ((sign) << 1)
#define PAGE_PDE_US(sign) ((sign) << 2)

#define PAGE_PDE_P_MASK (PAGE_PDE_P(1))
#define PAGE_PTE_P_MASK PAGE_PDE_P_MASK

#define PAGE_PDE_DESC(base, p, w, u)                                           \
  ((base) | PAGE_PDE_P(p) | PAGE_PDE_RW(w) | PAGE_PDE_US(u))
#define PAGE_PTE_DESC(base, p, w, u) PAGE_PDE_DESC(base, p, w, u)

#define PAGE_PDE_INDEX(addr) ((addr) >> 22)
#define PAGE_INDEX(addr) ((addr) >> 12)

#define PAGE_PDE_KERNEL_OFFSET PAGE_PDE_INDEX(MEM_KERNEL_VSTART)

/**
 * @brief Initialize the virtual address mapping
 */
void
init_paging(void);
