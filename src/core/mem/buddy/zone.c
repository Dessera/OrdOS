#include "ordos/core/mem/buddy/zone.h"
#include "ordos/core/mem.h"
#include "ordos/core/mem/bootmem.h"
#include "ordos/core/mem/buddy.h"
#include "ordos/core/mem/buddy/page.h"
#include "ordos/kernel/config.h"
#include "ordos/lib/common.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/sync.h"
#include "ordos/lib/types.h"

struct mem_zone __zones[3] = {};

static void
__add_free_block(size_t start, size_t cnt)
{
  size_t remaining = cnt;
  size_t curr = start;

  for (int order = ORDOS_MEM_BUDDY_MAX_ORDER; order >= 0 && remaining > 0;
       --order) {
    size_t blk_size = buddy_order_to_page_cnt(order);

    while (remaining >= blk_size && buddy_page_index_is_aligned(curr, order)) {
      struct page* pg = page_get(curr);
      buddy_free_page(pg, order);

      curr += blk_size;
      remaining -= blk_size;
    }
  }

  while (remaining > 0) {
    struct page* pg = page_get(curr);
    buddy_free_page(pg, 0);

    ++curr;
    --remaining;
  }
}

static void
__init_zone(struct mem_zone* zone,
            enum mem_type type,
            size_t pg_start,
            size_t pg_end)
{
  spin_lock_init(&zone->lock);

  zone->pg_free = 0;
  zone->pg_start = pg_start;
  if (pg_start > pg_end) {
    zone->pg_cnt = 0;
  } else {
    zone->pg_cnt = pg_end - pg_start + 1;
  }

  for (size_t i = 0; i <= ORDOS_MEM_BUDDY_MAX_ORDER; i++) {
    list_init(&zone->areas[i].mem_blocks);
    zone->areas[i].blocks_free = 0;
  }

  for (size_t i = pg_start; i <= pg_end; i++) {
    struct page* pg = page_get(i);
    pg->zone_type = type;
  }

  size_t curr = pg_start;
  while (curr <= pg_end) {
    struct page* pg = page_get(curr);

    if (pg->reserved) {
      ++curr;
      continue;
    }

    size_t blk_start = curr;
    size_t blk_cnt = 0;

    while (curr <= pg_end) {
      pg = page_get(curr);
      if (pg->reserved) {
        break;
      }
      ++blk_cnt;
      ++curr;
    }

    if (blk_cnt > 0) {
      __add_free_block(blk_start, blk_cnt);
    }
  }
}

void
init_zone(void)
{
  uintptr_t mem_size = bootmem_end();

  __init_zone(&__zones[MEM_DMA],
              MEM_DMA,
              page_phys_index(MEM_TYPE_DMA_START),
              page_phys_index(MEM_TYPE_NORMAL_START) - 1);

  __init_zone(&__zones[MEM_NORMAL],
              MEM_NORMAL,
              page_phys_index(MEM_TYPE_NORMAL_START),
              page_phys_index(min(MEM_TYPE_HIGH_START, mem_size)) - 1);

  __init_zone(&__zones[MEM_HIGH],
              MEM_HIGH,
              page_phys_index(MEM_TYPE_HIGH_START),
              page_phys_index(mem_size) - 1);

  kinfo("Zone DMA: %u mb, %u pages",
        page_size(__zones[MEM_DMA].pg_cnt, MBYTES),
        __zones[MEM_DMA].pg_cnt);
  kinfo("zone normal: %u mb, %u pages",
        page_size(__zones[MEM_NORMAL].pg_cnt, MBYTES),
        __zones[MEM_NORMAL].pg_cnt);
  kinfo("zone high: %u mb, %u pages",
        page_size(__zones[MEM_HIGH].pg_cnt, MBYTES),
        __zones[MEM_HIGH].pg_cnt);
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
