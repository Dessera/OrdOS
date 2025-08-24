#pragma once

#include "ordos/kernel/mem/buddy/page.h"
#include "ordos/lib/types.h"

/**
 * @brief Initializes the buddy memory allocator.
 */
void
init_buddy(void);

/**
 * @brief Free pages to the buddy allocator.
 *
 * @param page First page of the block.
 * @param order Order of the block.
 */
void
buddy_free_page(struct page* page, u8 order);

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
 * @brief Convert page count to order.
 *
 * @param page_cnt Page count.
 * @return u8 Order.
 */
u8
buddy_page_cnt_to_order(size_t page_cnt);
