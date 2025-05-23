#include "kernel/memory/buddy/zone.h"
#include "kernel/assert.h"
#include "kernel/log.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/vpage.h"
#include "kernel/task/sync.h"
#include "lib/common.h"
#include "lib/types.h"

struct mem_zone __zones[3];

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

  spin_lock_init(&zone->lock);
}

void
init_zone(void)
{
  uintptr_t mem_size = e820_get_memory_size();

  __init_zone(&__zones[MEM_ZONE_DMA],
              MEM_ZONE_DMA,
              PAGE_INDEX(MEM_TYPE_DMA_START),
              PAGE_INDEX(MEM_TYPE_NORMAL_START) - 1);
  KDEBUG("zone dma: %uMB, %u pages",
         __zones[MEM_ZONE_DMA].pg_cnt * MEMKB(4) / MEMMB(1),
         __zones[MEM_ZONE_DMA].pg_cnt);

  __init_zone(&__zones[MEM_ZONE_NORMAL],
              MEM_ZONE_NORMAL,
              PAGE_INDEX(MEM_TYPE_NORMAL_START),
              PAGE_INDEX(MIN(MEM_TYPE_HIGH_START, mem_size)) - 1);
  KDEBUG("zone normal: %uMB, %u pages",
         __zones[MEM_ZONE_NORMAL].pg_cnt * MEMKB(4) / MEMMB(1),
         __zones[MEM_ZONE_NORMAL].pg_cnt);

  __init_zone(&__zones[MEM_ZONE_HIGH],
              MEM_ZONE_HIGH,
              PAGE_INDEX(MEM_TYPE_HIGH_START),
              PAGE_INDEX(mem_size) - 1);
  KDEBUG("zone high: %uMB, %u pages",
         __zones[MEM_ZONE_HIGH].pg_cnt * MEMKB(4) / MEMMB(1),
         __zones[MEM_ZONE_HIGH].pg_cnt);
}

FORCE_INLINE struct mem_zone*
zone_get(enum mem_type type)
{
  return &__zones[type];
}

FORCE_INLINE enum mem_type
zone_get_type(struct mem_zone* zone)
{
  size_t mtype = zone - __zones;
  KASSERT(mtype < ARRAY_SIZE(__zones), "invalid memory zone type");
  return (enum mem_type)mtype;
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