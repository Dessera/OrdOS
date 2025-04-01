#include "kernel/memory/buddy/buddy.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/buddy/zone.h"
#include "kernel/memory/memory.h"
#include "lib/string.h" // IWYU pragma: keep
#include "lib/types.h"

static void
debug_mem_zone(struct mem_zone* zone)
{
  AUTO ztype = zone_get_type(zone);

  KINFO("memory zone %s : start %u, size %u, free %u",
        mem_type_to_str(ztype),
        zone->pg_start,
        zone->pg_cnt,
        zone->pg_free);

  for (u8 i = 0; i <= MEM_BUDDY_MAX_ORDER; i++) {
    AUTO area = zone->areas[i];
    KINFO("  order %u : free %u", i, area.blocks_free);

    if (area.blocks_free != 0) {
      AUTO p = buddy_alloc_page(ztype, i);
      KINFO("test allocate block of order %u : %p", i, page_get_virt(p));
      memset((void*)page_get_virt(p),
             42,
             buddy_order_to_page_cnt(i) * MEM_PAGE_SIZE);
      buddy_free_page(p, i);
    }
  }
}

DECLARE_WITH_PROTOTYPE(void, ktester, void)
{
  AUTO zone = zone_get(MEM_ZONE_NORMAL);
  debug_mem_zone(zone);
}
