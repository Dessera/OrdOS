#pragma once

#include "kernel/config/memory.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

#define MEM_ZONE_SIZE 3

/**
 * @brief Memory zone type
 */
enum mem_zone_type
{
  MEM_ZONE_DMA = 0,
  MEM_ZONE_NORMAL,
  MEM_ZONE_HIGH,
};

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

void
init_zone(void);