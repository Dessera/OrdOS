#pragma once

#include "ordos/kernel/config.h"
#include "ordos/kernel/mem.h"
#include "ordos/kernel/mem/buddy/page.h"
#include "ordos/kernel/task/sync.h"
#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/list_head.h"
#include "ordos/lib/types.h" // IWYU pragma: keep

#define MEM_TYPE_HIGH_START 0x30000000
#define MEM_TYPE_NORMAL_START 0x01000000
#define MEM_TYPE_DMA_START 0x00000000

/**
 * @brief Memory area.
 */
struct mem_area
{
  struct list_head mem_blocks;
  size_t blocks_free;
};

/**
 * @brief Memory zone.
 */
struct mem_zone
{
  struct mem_area areas[ORDOS_MEM_BUDDY_MAX_ORDER + 1];
  size_t pg_start;
  size_t pg_cnt;
  size_t pg_free;

  struct spin_lock lock;
};

extern struct mem_zone __zones[3];

/**
 * @brief Initialize the memory zones.
 */
void
init_zone(void);

/**
 * @brief Get the memory zone for a given type.
 *
 * @param type Memory type.
 * @return struct mem_zone* Memory zone.
 */
__inline struct mem_zone*
zone_get(enum mem_type type)
{
  return &__zones[type];
}

/**
 * @brief Get the memory type for a given zone.
 *
 * @param zone Memory zone.
 * @return enum mem_type Memory type.
 */
__inline static enum mem_type
zone_get_type(struct mem_zone* zone)
{
  return (enum mem_type)(zone - __zones);
}

/**
 * @brief Add a page to an area.
 *
 * @param area Memory area.
 * @param page Page to add.
 */
void
area_add_page(struct mem_area* area, struct page* page);

/**
 * @brief Remove a page from an area.
 *
 * @param area Memory area.
 * @param page Page to remove.
 */
void
area_remove_page(struct mem_area* area, struct page* page);
