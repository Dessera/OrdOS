#pragma once

#include "kernel/config/memory.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

/**
 * @brief sslab: a simple slab allocator
 */
struct sslab
{
  size_t obj_size;
  struct list_head caches;
};

void
init_sslab(void);

void
sslab_init(struct sslab* sslab, size_t obj_size);

void*
sslab_alloc(struct sslab* sslab);

void
sslab_free(struct sslab* sslab, void* obj);

void*
sslab_global_alloc(size_t size);

void
sslab_global_free(void* obj);

static FORCE_INLINE size_t
sslab_get_size_by_order(u8 order)
{
  return 1 << order;
}

static FORCE_INLINE u8
sslab_get_order_by_size(size_t size)
{
  u8 order = 0;
  while (size >>= 1) {
    order++;
  }

  return order;
}
