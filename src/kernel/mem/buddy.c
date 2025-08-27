#include "ordos/kernel/mem/buddy.h"
#include "ordos/kernel/assert.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/error.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem/buddy/page.h"
#include "ordos/kernel/mem/buddy/zone.h"
#include "ordos/kernel/module.h"
#include "ordos/kernel/task/sync.h"
#include "ordos/lib/list_head.h"
#include "ordos/lib/types.h"

/**
 * @brief Convert page to it's buddy.
 *
 */
static struct page*
__buddy_page_to_buddy(struct page* page, u8 order)
{
  size_t index = page_get_index(page) ^ (buddy_order_to_page_cnt(order));
  if (page_index_is_overflow(index)) {
    return NULL;
  }

  return page_get(page_get_index(page) ^ buddy_order_to_page_cnt(order));
}

/**
 * @brief Convert page to it's parent.
 *
 */
static struct page*
__buddy_page_ascend(struct page* page, u8 order)
{
  return page_get(page_get_index(page) & ~buddy_order_to_page_cnt(order));
}

void
buddy_free_page(struct page* page, u8 order)
{
  kassert(
    order <= ORDOS_MEM_BUDDY_MAX_ORDER, "Order too large, received %u", order);
  kassert(!page->reserved, "Cannot free reserved page %x", page_get_phys(page));
  kassert(!page->buddy,
          "Cannot free page %x that is part of a buddy block",
          page_get_phys(page));

  if (!buddy_page_is_aligned(page, order)) {
    kwarn("page %x is not aligned to order %x", page_get_phys(page), order);
    return;
  }

  struct mem_zone* zone = zone_get(page->zone_type);

  spin_lock(&zone->lock);

  zone->pg_free += buddy_order_to_page_cnt(order);

  struct mem_area* area = &zone->areas[order];
  while (order < ORDOS_MEM_BUDDY_MAX_ORDER) {
    struct page* buddy = __buddy_page_to_buddy(page, order);

    if (buddy == NULL || !buddy->buddy || buddy->order != order) {
      break;
    }

    area_remove_page(area, buddy);

    page = __buddy_page_ascend(page, order);
    order++;
    area = &zone->areas[order];
  }

  page->order = order;
  area_add_page(area, page);

  spin_unlock(&zone->lock);
}

struct page*
buddy_alloc_page(enum mem_type zone_type, u8 order)
{
  kassert(
    order <= ORDOS_MEM_BUDDY_MAX_ORDER, "order too large, received %u", order);
  struct mem_zone* zone = zone_get(zone_type);

  spin_lock(&zone->lock);

  size_t alloc_order = order;
  struct mem_area* area = NULL;
  while (alloc_order <= ORDOS_MEM_BUDDY_MAX_ORDER) {
    area = &zone->areas[alloc_order];
    if (area->blocks_free > 0) {
      break;
    }

    alloc_order++;
  }

  struct page* page = NULL;

  if (alloc_order > ORDOS_MEM_BUDDY_MAX_ORDER) {
    goto alloc_end;
  }

  page = list_entry(area->mem_blocks.next, struct page, node);
  kassert(page->buddy,
          "broken buddy system, page %x is not part of a block",
          page_get_phys(page));

  area_remove_page(area, page);

  while (alloc_order > order) {
    alloc_order--;
    area = &zone->areas[alloc_order];

    area_add_page(area, page);

    page->order = alloc_order;
    page = __buddy_page_to_buddy(page, alloc_order);
  }

  zone->pg_free -= buddy_order_to_page_cnt(order);

  kassert(!page->buddy,
          "broken buddy system, allocated page %x is still part of a block",
          page_get_phys(page));

alloc_end:
  spin_unlock(&zone->lock);
  return page;
}

bool
buddy_page_is_aligned(struct page* page, u8 order)
{
  return !(page_get_index(page) & ((buddy_order_to_page_cnt(order))-1));
}

bool
buddy_page_index_is_aligned(size_t index, u8 order)
{
  return !(index & ((buddy_order_to_page_cnt(order))-1));
}

u8
buddy_page_cnt_to_order(size_t page_cnt)
{
  u8 order = 0;
  size_t base = 1;
  while (base < page_cnt) {
    base <<= 1;
    order++;
  }
  return order;
}

int
buddy_entry(struct module* mod)
{
  (void)mod;

  init_page();
  init_zone();

  return E_SUCCESS;
}

module_init_noexit(sys_mem_buddy, MOD_COREMOD, buddy_entry)
