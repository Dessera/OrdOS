#pragma once

#include "kernel/memory/buddy/page.h"
#include "lib/types.h"

/**
 * @brief Initializes the buddy memory allocator
 */
void
init_buddy(void);

/**
 * @brief Free pages to the buddy allocator
 *
 * @param page First page of the block
 * @param order Order of the block
 */
void
buddy_free_page(struct page* page, u8 order);

/**
 * @brief Allocate a block of pages from the buddy allocator
 *
 * @param zone_type Type of memory zone
 * @param order Order of the block
 * @return struct page* First page of the block
 */
struct page*
buddy_alloc_page(enum mem_type zone_type, u8 order);

/**
 * @brief Check if a page is aligned to a given order
 *
 * @param page Page to check
 * @param order Order to check against
 * @return bool True if the page is aligned, false otherwise
 */
static FORCE_INLINE bool
buddy_page_is_aligned(struct page* page, u8 order)
{
  return !(page_get_index(page) & ((1 << order) - 1));
}

/**
 * @brief Get the number of pages in a block of a given order
 *
 * @param order Order of the block
 * @return size_t Number of pages in the block
 */
static FORCE_INLINE size_t
buddy_order_to_page_cnt(u8 order)
{
  return 1 << order;
}

/**
 * @brief Get the buddy of a page
 *
 * @param page Page to get the buddy of
 * @param order Order of the block
 * @return struct page* Buddy of the page
 */
static FORCE_INLINE struct page*
buddy_page_to_buddy(struct page* page, u8 order)
{
  AUTO index = page_get_index(page) ^ (1 << order);
  if (page_index_is_overflow(index)) {
    return nullptr;
  } else {
    return page_get(page_get_index(page) ^ (1 << order));
  }
}

/**
 * @brief Get the page at a given index
 *
 * @param page Page to get the index of
 * @param order Order of the block
 * @return struct page* Page at the given index
 */
static FORCE_INLINE struct page*
buddy_page_ascend(struct page* page, u8 order)
{
  return page_get(page_get_index(page) & ~(1 << order));
}
