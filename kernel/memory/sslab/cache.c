#include "kernel/memory/sslab/cache.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/memory/buddy.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/page.h"
#include "kernel/utils/list_head.h"
#include "lib/common.h"
#include "lib/types.h"

struct sslab_cache*
sslab_cache_create(size_t obj_size)
{
  AUTO page_phys = page_get_phys(buddy_alloc_page(MEM_ZONE_NORMAL, 0));
  struct sslab_cache* cache = (void*)MEM_KERNEL_VADDR(page_phys);

  // get alignment of obj_size and sizeof(struct sslab_cache)
  cache->object = ALIGN_UP(sizeof(struct sslab_cache), obj_size);

  for (u16 i = cache->object; i < MEM_PAGE_SIZE; i += obj_size) {
    struct sslab_object* obj = (void*)cache + i;
    obj->next = i + obj_size;
    if (obj->next >= MEM_PAGE_SIZE) {
      obj->next = 0;
    }
  }

  cache->obj_cnt = (MEM_PAGE_SIZE - cache->object) / obj_size;
  cache->obj_free = cache->obj_cnt;

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

  return obj;
}

void
sslab_cache_free(struct sslab_cache* cache, struct sslab_object* obj)
{
  u16 index = (void*)obj - (void*)cache;
  KASSERT(index < MEM_PAGE_SIZE, "sslab object index out of range");

  obj->next = cache->object;
  cache->object = index;
  cache->obj_free++;
}

void
sslab_cache_destroy(struct sslab_cache* cache)
{
  AUTO page = page_get_by_phys(MEM_KERNEL_PADDR(cache));
  buddy_free_page(page, 0);
}