#include "ordos/kernel/mem/sslab/cache.h"
#include "ordos/kernel/assert.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem.h"
#include "ordos/kernel/mem/buddy.h"
#include "ordos/kernel/mem/buddy/page.h"
#include "ordos/lib/common.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

struct sslab_cache*
sslab_cache_create(size_t obj_size)
{
  kassert(obj_size <= sslab_order_to_size(ORDOS_MEM_SSLAB_MAX_ORDER),
          "SSLAB: Object size too large");

  struct page* page = buddy_alloc_page(MEM_NORMAL, 0);
  if (page == NULL) {
    kwarn("SSLAB: Failed to allocate page");
    return NULL;
  }

  memset((void*)page_get_virt(page), 0, ORDOS_KERNEL_PAGE_SIZE);

  struct sslab_cache* cache = (void*)page_get_virt(page);
  cache->object = align_up(sizeof(struct sslab_cache), obj_size);

  for (u16 i = cache->object; i < ORDOS_KERNEL_PAGE_SIZE; i += obj_size) {
    struct sslab_object* obj = (void*)cache + i;
    obj->next = i + obj_size;
    if (obj->next >= ORDOS_KERNEL_PAGE_SIZE) {
      obj->next = 0;
    }
  }

  cache->obj_cnt = (ORDOS_KERNEL_PAGE_SIZE - cache->object) / obj_size;
  cache->obj_free = cache->obj_cnt;
  cache->obj_size = obj_size;

  return cache;
}

struct sslab_object*
sslab_cache_alloc(struct sslab_cache* cache)
{
  if (cache->obj_free == 0) {
    return NULL;
  }

  struct sslab_object* obj = (void*)cache + cache->object;
  cache->object = obj->next;
  cache->obj_free--;

  memset(obj, 0, cache->obj_size);
  return obj;
}

void
sslab_cache_free(struct sslab_cache* cache, struct sslab_object* obj)
{
  kassert(sslab_object_in_cache(obj, cache), "SSLAB: Object not in cache");

  obj->next = cache->object;
  cache->object = (void*)obj - (void*)cache;
  cache->obj_free++;
}

void
sslab_cache_destroy(struct sslab_cache* cache)
{
  buddy_free_page(page_get_by_virt((uintptr_t)cache), 0);
}
