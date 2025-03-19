#include "kernel/memory/buddy.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/page.h"
#include "kernel/utils/list_head.h"
#include "lib/common.h"
#include "lib/types.h"

struct mem_area
{
  struct list_head mem_blocks;
  size_t blocks_free;
};

struct mem_zone
{
  struct mem_area areas[MEM_BUDDY_MAX_ORDER + 1];
  size_t pg_start;
  size_t pg_cnt;
  size_t pg_free;
};

struct mem_zone __zones[MEM_ZONE_SIZE];

static void
__init_zone(struct mem_zone* zone,
            enum mem_zone_type type,
            size_t pg_start,
            size_t pg_end)
{
  zone->pg_free = 0;

  if (pg_start > pg_end) {
    zone->pg_cnt = 0;
  } else {
    zone->pg_cnt = pg_end - pg_start + 1;
  }

  zone->pg_start = pg_start;

  for (size_t i = 0; i <= MEM_BUDDY_MAX_ORDER; i++) {
    list_init(&zone->areas[i].mem_blocks);
    zone->areas[i].blocks_free = 0;
  }

  for (size_t i = pg_start; i <= pg_end; i++) {
    AUTO pg = page_get(i);
    pg->zone_type = type;

    if (!pg->reserved) {
      buddy_free_page(pg, 0);
    }
  }
}

static void
__area_add_page(struct mem_area* area, struct page* page)
{
  list_add(&page->node, &area->mem_blocks);
  area->blocks_free++;
  page->buddy = true;
}

static void
__area_delete_page(struct mem_area* area, struct page* page)
{
  list_del(&page->node);
  area->blocks_free--;
  page->buddy = false;
}

void
init_buddy(void)
{
  uintptr_t mem_size = e820_get_memory_size();

  // init zones
  __init_zone(&__zones[MEM_ZONE_DMA],
              MEM_ZONE_DMA,
              PAGE_INDEX(MEM_TYPE_DMA_START),
              PAGE_INDEX(MEM_TYPE_NORMAL_START) - 1);
  KDEBUG("buddy dma: %uMB, %u pages",
         __zones[MEM_ZONE_DMA].pg_cnt * MEMKB(4) / MEMMB(1),
         __zones[MEM_ZONE_DMA].pg_cnt);

  __init_zone(&__zones[MEM_ZONE_NORMAL],
              MEM_ZONE_NORMAL,
              PAGE_INDEX(MEM_TYPE_NORMAL_START),
              PAGE_INDEX(MIN(MEM_TYPE_HIGH_START, mem_size)) - 1);
  KDEBUG("buddy normal: %uMB, %u pages",
         __zones[MEM_ZONE_NORMAL].pg_cnt * MEMKB(4) / MEMMB(1),
         __zones[MEM_ZONE_NORMAL].pg_cnt);

  __init_zone(&__zones[MEM_ZONE_HIGH],
              MEM_ZONE_HIGH,
              PAGE_INDEX(MEM_TYPE_HIGH_START),
              PAGE_INDEX(mem_size) - 1);
  KDEBUG("buddy high: %uMB, %u pages",
         __zones[MEM_ZONE_HIGH].pg_cnt * MEMKB(4) / MEMMB(1),
         __zones[MEM_ZONE_HIGH].pg_cnt);

  AUTO p = buddy_alloc_page(MEM_ZONE_NORMAL, 5);
  KINFO("buddy system initialized, first page: %u", page_get_index(p));
  buddy_free_page(p, 5);
}

void
buddy_free_page(struct page* page, u8 order)
{
  KASSERT(page->ref_cnt == 0,
          "page %p is still in use, ref_cnt = %u",
          page_get_phys(page),
          page->ref_cnt);
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

  AUTO zone = &__zones[page->zone_type];
  zone->pg_free += BUDDY_ORDER_PAGES(order);

  AUTO area = &zone->areas[order];
  while (order < MEM_BUDDY_MAX_ORDER) {
    AUTO buddy_idx = BUDDY_GET_BUDDY_INDEX(idx, order);
    AUTO buddy = page_get(buddy_idx);

    if (!buddy->buddy) {
      break;
    }

    // if mismatched order, assume that buddy system is broken
    KASSERT(buddy->order == order,
            "broken buddy system, buddy %p is not of order %u",
            page_get_phys(buddy),
            order);

    __area_delete_page(area, buddy);

    idx = BUDDY_GET_ASCEND_INDEX(idx, order);
    order++;
    area = &zone->areas[order];
  }

  page = page_get(idx);
  page->order = order;
  __area_add_page(area, page);
}

struct page*
buddy_alloc_page(enum mem_zone_type zone_type, u8 order)
{
  KASSERT(order <= MEM_BUDDY_MAX_ORDER, "order too large, received %u", order);
  AUTO zone = &__zones[zone_type];

  struct mem_area* area = &zone->areas[order];
  size_t alloc_order = order;
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

  __area_delete_page(area, page);

  while (alloc_order > order) {
    alloc_order--;
    area = &zone->areas[alloc_order];

    __area_add_page(area, page);

    page->order = alloc_order;
    page += 1 << alloc_order;
  }

  zone->pg_free -= BUDDY_ORDER_PAGES(order);

  KASSERT(!page->buddy,
          "broken buddy system, allocated page %p is still part of a block",
          page_get_phys(page));
  return page;
}
