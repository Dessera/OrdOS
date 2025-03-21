#pragma once

#include "kernel/memory/buddy/page.h"
#include "lib/types.h"

#define BUDDY_ALIGN_CHECK(idx, order) (!((idx) & ((1 << (order)) - 1)))
#define BUDDY_ORDER_PAGES(order) (1 << (order))
#define BUDDY_GET_BUDDY_INDEX(idx, order) ((idx) ^ (1 << (order)))
#define BUDDY_GET_ASCEND_INDEX(idx, order) ((idx) & ~(1 << (order)))

void
init_buddy(void);

void
buddy_free_page(struct page* page, u8 order);

struct page*
buddy_alloc_page(enum mem_zone_type zone_type, u8 order);
