/**
 * @file cache.h
 * @author Dessera (dessera@qq.com)
 * @brief SSLAB Cache pool.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/config.h"
#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"
#include "ordos/lib/util/list_head.h"

/**
 * @brief Object in memory.
 */
struct sslab_object
{
  u16 next;
};

/**
 * @brief Cache for objects.
 */
struct sslab_cache
{
  size_t obj_cnt;
  size_t obj_size;
  size_t obj_free;

  struct list_head node;
  u16 object;
};

/**
 * @brief Create a cache.
 *
 * @param obj_size Size of the objects in the cache.
 * @return struct sslab_cache* Created cache.
 */
struct sslab_cache*
sslab_cache_create(size_t obj_size);

/**
 * @brief Destroy a cache.
 *
 * @param cache Cache to destroy.
 */
void
sslab_cache_destroy(struct sslab_cache* cache);

/**
 * @brief Allocate an object from a cache.
 *
 * @param cache Cache to allocate from.
 * @return struct sslab_object* Allocated object.
 */
struct sslab_object*
sslab_cache_alloc(struct sslab_cache* cache);

/**
 * @brief Free an object from a cache.
 *
 * @param cache Cache to free the object from.
 * @param obj Object to free.
 */
void
sslab_cache_free(struct sslab_cache* cache, struct sslab_object* obj);

/**
 * @brief Check if an object is in a cache.
 *
 * @param obj Object to check.
 * @param cache Cache to check in.
 * @return true if the object is in the cache.
 * @return false if the object is not in the cache.
 */
__inline static bool
sslab_object_in_cache(struct sslab_object* obj, struct sslab_cache* cache)
{
  return ((void*)obj - (void*)cache) < ORDOS_KERNEL_PAGE_SIZE;
}

/**
 * @brief Get the cache of a given object.
 *
 * @param obj Object to get the cache of.
 * @return struct sslab_cache* Cache of the object.
 */
__inline static struct sslab_cache*
sslab_object_to_cache(struct sslab_object* obj)
{
  return (struct sslab_cache*)((uintptr_t)obj & ~(ORDOS_KERNEL_PAGE_SIZE - 1));
}
