#include "kernel/memory/buddy.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/page.h"
#include "kernel/utils/compiler.h"
#include "kernel/utils/list_head.h"
#include "kernel/utils/string.h"
#include "lib/common.h"
#include "lib/types.h"

#define BUDDY_ALIGN_CHECK(idx, order) (!(idx & ((1 << order) - 1)))

static struct page* __pages;

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
    AUTO pg = &__pages[i];
    pg->zone_type = type;

    if (!pg->reserved) {
      buddy_free_page(pg, 0);
    }
  }
}

void
init_buddy(void)
{
  uintptr_t mem_size = e820_get_memory_size();
  size_t mem_pages = DIV_DOWN(mem_size, MEM_PAGE_SIZE);
  __pages = bootmem_alloc((sizeof(struct page) * mem_pages));
  kmemset(__pages, 0, sizeof(struct page) * mem_pages);

  e820_pre_init_pages(__pages, mem_pages);
  bootmem_pre_init_pages(__pages, mem_pages);

  // init zones
  __init_zone(&__zones[MEM_ZONE_DMA],
              MEM_ZONE_DMA,
              PAGE_INDEX(MEM_TYPE_DMA_START),
              PAGE_INDEX(MEM_TYPE_NORMAL_START) - 1);
  __init_zone(&__zones[MEM_ZONE_NORMAL],
              MEM_ZONE_NORMAL,
              PAGE_INDEX(MEM_TYPE_NORMAL_START),
              PAGE_INDEX(MIN(MEM_TYPE_HIGH_START, mem_size)) - 1);
  __init_zone(&__zones[MEM_ZONE_HIGH],
              MEM_ZONE_HIGH,
              PAGE_INDEX(MEM_TYPE_HIGH_START),
              PAGE_INDEX(mem_size) - 1);

  KDEBUG("buddy dma: %u pages", __zones[MEM_ZONE_DMA].pg_cnt);
  KDEBUG("buddy normal: %u pages", __zones[MEM_ZONE_NORMAL].pg_cnt);
  KDEBUG("buddy high: %u pages", __zones[MEM_ZONE_HIGH].pg_cnt);
}

void
buddy_free_page(struct page* page, u8 order)
{
  KASSERT(page->ref_cnt == 0,
          "page %p is still in use, ref_cnt = %u",
          buddy_get_page_paddr(page),
          page->ref_cnt);
  KASSERT(order <= MEM_BUDDY_MAX_ORDER, "order too large, received %u", order);
  KASSERT(!page->reserved,
          "cannot free reserved page %p",
          buddy_get_page_paddr(page));
  KASSERT(!page->buddy,
          "cannot free page %p that is part of a buddy block",
          buddy_get_page_paddr(page));

  AUTO idx = buddy_get_page_index(page);
  if (!BUDDY_ALIGN_CHECK(idx, order)) {
    return;
  }

  AUTO zone = &__zones[page->zone_type];
  zone->pg_free += 1 << order;

  while (order < MEM_BUDDY_MAX_ORDER) {
    AUTO area = &zone->areas[order];
    AUTO buddy_idx = idx ^ (1 << order);
    AUTO buddy = &__pages[buddy_idx];

    if (!buddy->buddy || buddy->order != order) {
      break;
    }

    area->blocks_free--;
    buddy->buddy = false;
    list_del(&buddy->node);

    idx &= ~(1 << order);
    order++;
  }

  AUTO area = &zone->areas[order];
  area->blocks_free++;
  page->buddy = true;
  page->order = order;
  list_add(&page->node, &area->mem_blocks);
}

size_t
buddy_get_page_index(struct page* page)
{
  return page - __pages;
}

uintptr_t
buddy_get_page_paddr(struct page* page)
{
  return buddy_get_page_index(page) * MEM_PAGE_SIZE;
}