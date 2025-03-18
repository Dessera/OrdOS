#pragma once

#include "kernel/memory/memory.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

struct page
{
  size_t ref_cnt;
  u8 order;
  struct list_head node;
  enum mem_zone_type zone_type;
  bool reserved;
  bool buddy;
};

void
init_buddy(void);

void
buddy_free_page(struct page* page, u8 order);

struct page*
buddy_alloc_page(enum mem_zone_type zone_type, u8 order);

size_t
buddy_get_page_index(struct page* page);

uintptr_t
buddy_get_page_paddr(struct page* page);