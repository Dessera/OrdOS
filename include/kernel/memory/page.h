#pragma once

#include "kernel/config/memory.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

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
 * @brief Physical memory page abstraction
 */
struct page
{
  u8 order;
  enum mem_zone_type zone_type;

  struct list_head node;
  bool reserved;
  bool buddy;
};

/**
 * @brief Initialize the virtual address mapping and physical memory management
 */
void
init_page(void);

/**
 * @brief Get the index of the page
 *
 * @param page page to get the index of
 * @return size_t index of the page
 */
size_t
page_get_index(struct page* page);

/**
 * @brief Get the page from the index
 *
 * @param index index of the page
 * @return struct page* page
 */
struct page*
page_get(size_t index);

/**
 * @brief Get the physical address of the page
 *
 * @param page page to get the physical address of
 * @return uintptr_t physical address of the page
 */
uintptr_t
page_get_phys(struct page* page);

/**
 * @brief Get the page from the physical address
 *
 * @param phys physical address of the page
 * @return struct page* page
 */
struct page*
page_get_by_phys(uintptr_t phys);