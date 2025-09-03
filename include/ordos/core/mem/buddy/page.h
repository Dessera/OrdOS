/**
 * @file page.h
 * @author Dessera (dessera@qq.com)
 * @brief Physical page abstraction.
 * @version 0.1.0
 * @date 2025-08-23
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/config.h"
#include "ordos/core/mem.h"
#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"
#include "ordos/lib/util/list_head.h"

/**
 * @brief Physical memory page abstraction.
 */
struct page
{
  enum mem_type zone_type;
  u8 order;
  bool reserved;
  bool buddy;

  struct list_head node;
};

extern struct page* __pages; /**< Physical memory pages. */

extern size_t __pages_cnt; /**< Physical memory pages count. */

/**
 * @brief Initialize the physical memory page abstraction.
 */
void
init_page(void);

/**
 * @brief Get the index of the page.
 *
 * @param page Page to get the index of.
 * @return size_t Index of the page.
 */
__inline static size_t
page_index(struct page* page)
{
  return page - __pages;
}

/**
 * @brief Get page index from physical address.
 *
 * @param addr Physical address.
 * @return size_t Page index.
 */
__inline static size_t
page_phys_index(uintptr_t addr)
{
  return addr / ORDOS_KERNEL_PAGE_SIZE;
}

/**
 * @brief Check if index is overflow.
 *
 * @param index Page index.
 * @return true If index is overflow, otherwise false.
 */
__inline static bool
page_index_is_overflow(size_t index)
{
  return index >= __pages_cnt;
}

/**
 * @brief Get the page from the index
 *
 * @param index index of the page
 * @return struct page* page
 */
__inline static struct page*
page_get(size_t index)
{
  return &__pages[index];
}

/**
 * @brief Get the physical address of the page.
 *
 * @param page Page to get the physical address of.
 * @return uintptr_t Physical address of the page.
 */
__inline static uintptr_t
page_get_phys(struct page* page)
{
  return page_index(page) * ORDOS_KERNEL_PAGE_SIZE;
}

/**
 * @brief Get the page from physical address.
 *
 * @param phys Physical address of the page.
 * @return struct page* Memory page.
 */
__inline static struct page*
page_get_by_phys(uintptr_t phys)
{
  return page_get(phys / ORDOS_KERNEL_PAGE_SIZE);
}

/**
 * @brief Get the virtual address of the page (kernel only).
 *
 * @param page Page to get the virtual address of.
 * @return uintptr_t Virtual address of the page.
 */
__inline static uintptr_t
page_get_virt(struct page* page)
{
  return page_get_phys(page) + ORDOS_KERNEL_VADDR;
}

/**
 * @brief Get the page from virtual address.
 *
 * @param phys Virtual address of the page.
 * @return struct page* Memory page.
 */
__inline static struct page*
page_get_by_virt(uintptr_t virt)
{
  return page_get_by_phys(virt - ORDOS_KERNEL_VADDR);
}

/**
 * @brief Get memory size from pages.
 *
 * @param pages_cnt Pages count.
 * @return size_t Memory size.
 */
__inline static size_t
page_size(size_t pages_cnt, size_t base)
{
  return pages_cnt * ORDOS_KERNEL_PAGE_SIZE / base;
}
