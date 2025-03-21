#include "kernel/memory/buddy.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/buddy/zone.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

void
init_buddy(void)
{
  init_page();
  init_zone();
}

void
buddy_free_page(struct page* page, u8 order)
{
  KASSERT(order <= MEM_BUDDY_MAX_ORDER, "order too large, received %u", order);
  KASSERT(!page->reserved, "cannot free reserved page %p", page_get_phys(page));
  KASSERT(!page->buddy,
          "cannot free page %p that is part of a buddy block",
          page_get_phys(page));

  AUTO idx = page_get_index(page);
  if (!BUDDY_ALIGN_CHECK(idx, order)) {
    KWARNING("page %p is not aligned to order %u", page_get_phys(page), order);
    return;
  }

  AUTO zone = zone_get(page->zone_type);
  zone->pg_free += BUDDY_ORDER_PAGES(order);

  AUTO area = &zone->areas[order];
  while (order < MEM_BUDDY_MAX_ORDER) {
    AUTO buddy_idx = BUDDY_GET_BUDDY_INDEX(idx, order);
    AUTO buddy = page_get(buddy_idx);

    if (!buddy->buddy || buddy->order != order) {
      break;
    }

    area_remove_page(area, buddy);

    idx = BUDDY_GET_ASCEND_INDEX(idx, order);
    order++;
    area = &zone->areas[order];
  }

  // final page
  page = page_get(idx);
  page->order = order;
  area_add_page(area, page);
}

struct page*
buddy_alloc_page(enum mem_type zone_type, u8 order)
{
  KASSERT(order <= MEM_BUDDY_MAX_ORDER, "order too large, received %u", order);
  AUTO zone = zone_get(zone_type);

  size_t alloc_order = order;
  struct mem_area* area;
  while (alloc_order <= MEM_BUDDY_MAX_ORDER) {
    area = &zone->areas[alloc_order];
    if (area->blocks_free > 0) {
      break;
    }

    alloc_order++;
  }

  if (alloc_order > MEM_BUDDY_MAX_ORDER) {
    return NULL;
  }

  AUTO page = LIST_ENTRY(area->mem_blocks.next, struct page, node);
  KASSERT(page->buddy,
          "broken buddy system, page %p is not part of a block",
          page_get_phys(page));

  area_remove_page(area, page);

  while (alloc_order > order) {
    alloc_order--;
    area = &zone->areas[alloc_order];

    area_add_page(area, page);

    page->order = alloc_order;
    page += 1 << alloc_order;
  }

  zone->pg_free -= BUDDY_ORDER_PAGES(order);

  KASSERT(!page->buddy,
          "broken buddy system, allocated page %p is still part of a block",
          page_get_phys(page));
  return page;
}
