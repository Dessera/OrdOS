#include "kernel/memory/buddy.h"
#include "kernel/config/memory.h"
#include "kernel/log.h"
#include "kernel/memory/bootmem.h"
#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/compiler.h"
#include "kernel/utils/list_head.h"
#include "kernel/utils/string.h"
#include "lib/common.h"
#include "lib/types.h"

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

void
init_buddy(void)
{
  size_t mem_pages = DIV_DOWN(e820_get_memory_size(), MEM_PAGE_SIZE);
  __pages = bootmem_alloc((sizeof(struct page) * mem_pages));
  kmemset(__pages, 0, sizeof(struct page) * mem_pages);

  e820_pre_init_pages(__pages, mem_pages);
  bootmem_pre_init_pages(__pages, mem_pages);
}