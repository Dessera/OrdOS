#include "ordos/kernel/mem/buddy/zone.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem/bootmem.h"
#include "ordos/kernel/mem/buddy/buddy.h"
#include "ordos/kernel/mem/buddy/page.h"
#include "ordos/kernel/mem/memory.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

struct mem_zone __zones[3] = {};

static void
__init_zone(struct mem_zone* zone,
            enum mem_type type, // NOLINT
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

  for (size_t i = 0; i <= ORDOS_MEM_MAX_ORDER; i++) {
    list_init(&zone->areas[i].mem_blocks);
    zone->areas[i].blocks_free = 0;
  }

  for (size_t i = pg_start; i <= pg_end; i++) {
    struct page* pg = page_get(i);
    pg->zone_type = type;

    if (!pg->reserved) {
      buddy_free_page(pg, 0);
    }
  }

  // spin_lock_init(&zone->lock);
}

void
init_zone(void)
{
  uintptr_t mem_size = bootmem_end();

  __init_zone(&__zones[MEM_DMA],
              MEM_DMA,
              page_get_index_by_phys(MEM_TYPE_DMA_START),
              page_get_index_by_phys(MEM_TYPE_NORMAL_START) - 1);
  kdebug("Zone DMA: %u pages", __zones[MEM_DMA].pg_cnt);

  __init_zone(&__zones[MEM_NORMAL],
              MEM_NORMAL,
              page_get_index_by_phys(MEM_TYPE_NORMAL_START),
              page_get_index_by_phys(min(MEM_TYPE_HIGH_START, mem_size)) - 1);
  kdebug("zone normal: %u pages", __zones[MEM_NORMAL].pg_cnt);

  __init_zone(&__zones[MEM_HIGH],
              MEM_HIGH,
              page_get_index_by_phys(MEM_TYPE_HIGH_START),
              page_get_index_by_phys(mem_size) - 1);
  kdebug("zone high: %u pages", __zones[MEM_HIGH].pg_cnt);
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
