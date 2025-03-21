#pragma once

#include "kernel/utils/list_head.h"
#include "lib/types.h"

/**
 * @brief Object in memory
 */
struct sslab_object
{
  u16 next;
};

/**
 * @brief Cache for objects
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
 * @brief Create a cache
 *
 * @param obj_size Size of the objects in the cache
 * @return struct sslab_cache* Created cache
 */
struct sslab_cache*
sslab_cache_create(size_t obj_size);

/**
 * @brief Destroy a cache
 *
 * @param cache Cache to destroy
 */
void
sslab_cache_destroy(struct sslab_cache* cache);

/**
 * @brief Allocate an object from a cache
 *
 * @param cache Cache to allocate from
 * @return struct sslab_object* Allocated object
 */
struct sslab_object*
sslab_cache_alloc(struct sslab_cache* cache);

/**
 * @brief Free an object from a cache
 *
 * @param cache Cache to free the object from
 * @param obj Object to free
 */
void
sslab_cache_free(struct sslab_cache* cache, struct sslab_object* obj);

/**
 * @brief Check if an object is in a cache
 *
 * @param obj Object to check
 * @param cache Cache to check in
 * @return true if the object is in the cache
 * @return false if the object is not in the cache
 */
bool
sslab_object_in_cache(struct sslab_object* obj, struct sslab_cache* cache);

/**
 * @brief Get the cache of a given object
 *
 * @param obj Object to get the cache of
 * @return struct sslab_cache* Cache of the object
 */
struct sslab_cache*
sslab_object_to_cache(struct sslab_object* obj);
