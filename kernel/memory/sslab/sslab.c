#include "kernel/memory/sslab/sslab.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/memory/sslab/cache.h"
#include "kernel/utils/list_head.h"

static struct sslab __sslab[MEM_SSLAB_MAX_ORDER + 1];

void
init_sslab(void)
{
  for (int i = 0; i <= MEM_SSLAB_MAX_ORDER; i++) {
    sslab_init(&__sslab[i], sslab_order_to_size(i));
  }
}

void
sslab_init(struct sslab* sslab, size_t obj_size)
{
  list_init(&sslab->caches);
  sslab->obj_size = obj_size;
}

void*
sslab_alloc(struct sslab* sslab)
{
  struct list_head* pos;
  struct sslab_cache* cache = NULL;
  LIST_FOR_EACH(pos, &sslab->caches)
  {
    AUTO target_cache = LIST_ENTRY(pos, struct sslab_cache, node);
    if (target_cache->obj_free > 0) {
      cache = target_cache;
      break;
    }
  }

  if (cache == NULL) {
    cache = sslab_cache_create(sslab->obj_size);
    if (cache == NULL) {
      return NULL;
    }

    list_add_tail(&cache->node, &sslab->caches);
  }

  return sslab_cache_alloc(cache);
}

void
sslab_free(struct sslab* sslab, void* obj)
{
  struct list_head* pos;
  struct sslab_cache* cache = NULL;
  LIST_FOR_EACH(pos, &sslab->caches)
  {
    AUTO target_cache = LIST_ENTRY(pos, struct sslab_cache, node);
    if (sslab_object_in_cache(obj, target_cache)) {
      cache = target_cache;
      break;
    }
  }

  KASSERT(cache != NULL, "free object that is not in this sslab");

  sslab_cache_free(cache, obj);

  if (cache->obj_free == cache->obj_cnt) {
    list_del(&cache->node);
    sslab_cache_destroy(cache);
  }
}

void*
sslab_global_alloc(size_t size)
{
  AUTO order = sslab_size_to_order(size);
  KASSERT(order <= MEM_SSLAB_MAX_ORDER, "too large object to alloc");

  return sslab_alloc(&__sslab[order]);
}

void
sslab_global_free(void* obj)
{
  AUTO cache = sslab_object_to_cache(obj);

  AUTO order = sslab_size_to_order(cache->obj_size);
  KASSERT(order <= MEM_SSLAB_MAX_ORDER, "too large object to free");

  sslab_free(&__sslab[order], obj);
}

FORCE_INLINE size_t
sslab_order_to_size(u8 order)
{
  return 1 << order;
}

FORCE_INLINE u8
sslab_size_to_order(size_t size)
{
  u8 order = 0;
  while (size >>= 1) {
    order++;
  }

  return order;
}