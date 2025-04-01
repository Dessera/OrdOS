#pragma once

#include "kernel/config/memory.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/memory.h"
#include "kernel/task/sync.h"
#include "lib/list_head.h"

/**
 * @brief Memory area
 */
struct mem_area
{
  struct list_head mem_blocks;
  size_t blocks_free;
};

/**
 * @brief Memory zone
 */
struct mem_zone
{
  struct mem_area areas[MEM_BUDDY_MAX_ORDER + 1];
  size_t pg_start;
  size_t pg_cnt;
  size_t pg_free;

  struct spin_lock lock;
};

/**
 * @brief Initialize the memory zones
 */
void
init_zone(void);

/**
 * @brief Get the memory zone for a given type
 *
 * @param type Memory type
 * @return struct mem_zone* Memory zone
 */
struct mem_zone*
zone_get(enum mem_type type);

/**
 * @brief Get the memory type for a given zone
 *
 * @param zone Memory zone
 * @return enum mem_type Memory type
 */
enum mem_type
zone_get_type(struct mem_zone* zone);

/**
 * @brief Add a page to an area
 *
 * @param area Memory area
 * @param page Page to add
 */
void
area_add_page(struct mem_area* area, struct page* page);

/**
 * @brief Remove a page from an area
 *
 * @param area Memory area
 * @param page Page to remove
 */
void
area_remove_page(struct mem_area* area, struct page* page);