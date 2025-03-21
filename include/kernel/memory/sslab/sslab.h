#pragma once

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

size_t
sslab_order_to_size(u8 order);

u8
sslab_size_to_order(size_t size);
