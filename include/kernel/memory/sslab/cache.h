#pragma once

#include "kernel/config/memory.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

struct sslab_object
{
  u16 next;
};

struct sslab_cache
{
  size_t obj_cnt;
  size_t obj_size;
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

static FORCE_INLINE bool
sslab_object_in_cache(struct sslab_object* obj, struct sslab_cache* cache)
{
  return ((void*)obj - (void*)cache) < MEM_PAGE_SIZE;
}

static FORCE_INLINE struct sslab_cache*
sslab_object_cache(struct sslab_object* obj)
{
  // sslab cache is located at the beginning of the page
  return (struct sslab_cache*)((uintptr_t)obj & ~(MEM_PAGE_SIZE - 1));
}
