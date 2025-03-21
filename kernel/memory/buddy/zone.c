#include "kernel/memory/buddy/zone.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/page.h"
#include "lib/common.h"

struct mem_zone __zones[MEM_ZONE_SIZE];

static void
__init_zone(struct mem_zone* zone,
            enum mem_type type,
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

void
init_zone(void)
{
  uintptr_t mem_size = e820_get_memory_size();

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
}

struct mem_zone*
zone_get(enum mem_type type)
{
  return &__zones[type];
}

enum mem_type
zone_get_type(struct mem_zone* zone)
{
  return zone - __zones;
}

void
area_add_page(struct mem_area* area, struct page* page)
{
  list_add(&page->node, &area->mem_blocks);
  area->blocks_free++;
  page->buddy = true;
}

void
area_remove_page(struct mem_area* area, struct page* page)
{
  list_del(&page->node);
  area->blocks_free--;
  page->buddy = false;
}