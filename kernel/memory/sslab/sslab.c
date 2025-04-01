#include "kernel/memory/sslab/sslab.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/memory/sslab/cache.h"
#include "kernel/task/sync.h"
#include "lib/list_head.h"

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
  KASSERT(obj_size <= sslab_order_to_size(MEM_SSLAB_MAX_ORDER),
          "too large object size to initialize sslab");

  list_init(&sslab->caches);
  spin_lock_init(&sslab->lock);
  sslab->obj_size = obj_size;
}

void*
sslab_alloc(struct sslab* sslab)
{
  spin_lock(&sslab->lock);

  struct list_head* pos;
  struct sslab_cache* cache = nullptr;
  LIST_FOR_EACH(pos, &sslab->caches)
  {
    AUTO target_cache = LIST_ENTRY(pos, struct sslab_cache, node);
    if (target_cache->obj_free > 0) {
      cache = target_cache;
      break;
    }
  }

  void* obj = nullptr;

  if (cache == nullptr) {
    cache = sslab_cache_create(sslab->obj_size);
    if (cache == nullptr) {
      goto alloc_end;
    }

    list_add_tail(&cache->node, &sslab->caches);
  }

  obj = sslab_cache_alloc(cache);

alloc_end:
  spin_unlock(&sslab->lock);
  return obj;
}

void
sslab_free(struct sslab* sslab, void* obj)
{
  spin_lock(&sslab->lock);

  struct list_head* pos;
  struct sslab_cache* cache = nullptr;
  LIST_FOR_EACH(pos, &sslab->caches)
  {
    AUTO target_cache = LIST_ENTRY(pos, struct sslab_cache, node);
    if (sslab_object_in_cache(obj, target_cache)) {
      cache = target_cache;
      break;
    }
  }

  KASSERT(cache != nullptr, "free object that is not in this sslab");

  sslab_cache_free(cache, obj);

  if (cache->obj_free == cache->obj_cnt) {
    list_del(&cache->node);
    sslab_cache_destroy(cache);
  }

  spin_unlock(&sslab->lock);
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
