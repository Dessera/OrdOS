#pragma once

#include "ordos/kernel/mem/buddy/page.h"
#include "ordos/lib/types.h"

/**
 * @brief Allocate a block of pages from the buddy allocator.
 *
 * @param zone_type Type of memory zone.
 * @param order Order of the block.
 * @return struct page* First page of the block.
 */
struct page*
buddy_alloc_page(enum mem_type zone_type, u8 order);

/**
 * @brief Free pages to the buddy allocator.
 *
 * @param page First page of the block.
 * @param order Order of the block.
 */
void
buddy_free_page(struct page* page, u8 order);

/**
 * @brief Check if page is aligned to order.
 *
 * @param page Page struct.
 * @param order Order.
 * @return true If page is aligned, otherwise false.
 */
bool
buddy_page_is_aligned(struct page* page, u8 order);

/**
 * @brief Check if page index is aligned to order.
 *
 * @param index Page index.
 * @param order Order.
 * @return true If page is aligned, otherwise false.
 */
bool
buddy_page_index_is_aligned(size_t index, u8 order);

/**
 * @brief Convert page count to order.
 *
 * @param page_cnt Page count.
 * @return u8 Order.
 */
u8
buddy_page_cnt_to_order(size_t page_cnt);

/**
 * @brief Convert order to pages count.
 *
 * @param order Order.
 * @return size_t Page count.
 */
__inline static size_t
buddy_order_to_page_cnt(u8 order)
{
  return 1 << order;
}
