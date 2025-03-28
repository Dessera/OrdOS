#pragma once

#include "kernel/config/memory.h"
#include "lib/types.h"

#define PAGE_DESC_SIZE 4

#define PAGE_ENTRIES (MEM_PAGE_SIZE / PAGE_DESC_SIZE)

#define PAGE_PADDR(addr) ((addr) & ~(MEM_PAGE_SIZE - 1))

#define PAGE_PDE_DESC(base, p, w, u) ((base) | (p) | ((w) << 1) | ((u) << 2))
#define PAGE_PTE_DESC(base, p, w, u) PAGE_PDE_DESC(base, p, w, u)

#define PAGE_PDE_INDEX(addr) ((addr) >> 22)
#define PAGE_PTE_INDEX(addr) ((addr) >> 12 & 0x3ff)
#define PAGE_INDEX(addr) ((addr) >> 12)

#define PAGE_PDE_KERNEL_OFFSET PAGE_PDE_INDEX(MEM_KERNEL_VSTART)

/**
 * @brief Initialize the virtual address mapping
 */
void
init_vpage(void);

/**
 * @brief Get virtual address of kernel page table
 *
 * @return u32* Virtual address of kernel page table
 */
u32*
vpage_kernel_vaddr(void);

/**
 * @brief Get physical address of kernel page table
 *
 * @return uintptr_t Physical address of kernel page table
 */
uintptr_t
vpage_kernel_paddr(void);

/**
 * @brief Link virtual address to any physical address
 *
 * @param vaddr Virtual address to link
 */
void*
vpage_link_addr(uintptr_t vaddr);
