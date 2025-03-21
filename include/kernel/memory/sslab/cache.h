#pragma once

#include "kernel/utils/list_head.h"
#include "lib/types.h"

struct sslab_object
{
  u16 next;
};

struct sslab_cache
{
  size_t obj_cnt;
  size_t obj_free;

  struct list_head node;
  u16 object;
};

struct sslab_cache*
sslab_cache_create(size_t obj_size);

void
sslab_cache_destroy(struct sslab_cache* cache);

struct sslab_object*
sslab_cache_alloc(struct sslab_cache* cache);

void
sslab_cache_free(struct sslab_cache* cache, struct sslab_object* obj);
