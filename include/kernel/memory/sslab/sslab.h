#pragma once

#include "kernel/task/sync.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

/**
 * @brief SSLAB: a simple slab allocator
 */
struct sslab
{
  size_t obj_size;
  struct list_head caches;

  struct spin_lock lock;
};

/**
 * @brief Initialize the global sslab allocator
 */
void
init_sslab(void);

/**
 * @brief Initialize a sslab allocator
 *
 * @param sslab The sslab allocator
 * @param obj_size The size of each object
 */
void
sslab_init(struct sslab* sslab, size_t obj_size);

/**
 * @brief Allocate an object from the sslab allocator
 *
 * @param sslab The sslab allocator
 * @return void* The allocated object
 */
void*
sslab_alloc(struct sslab* sslab);

/**
 * @brief Free an object to the sslab allocator
 *
 * @param sslab The sslab allocator
 * @param obj The object to free
 */
void
sslab_free(struct sslab* sslab, void* obj);

/**
 * @brief Allocate an object from the global sslab allocator
 *
 * @param size The size of the object
 * @return void* The allocated object
 */
void*
sslab_global_alloc(size_t size);

/**
 * @brief Free an object to the global sslab allocator
 *
 * @param obj The object to free
 */
void
sslab_global_free(void* obj);

/**
 * @brief Convert order to size
 *
 * @param order The order
 * @return size_t The size
 */
static FORCE_INLINE size_t
sslab_order_to_size(u8 order)
{
  return 1 << order;
}

/**
 * @brief Convert size to order
 *
 * @param size The size
 * @return u8 The order
 * @warning This method consumes time
 */
static FORCE_INLINE u8
sslab_size_to_order(size_t size)
{
  u8 order = 0;
  size_t base = 1;
  while (base < size) {
    base <<= 1;
    order++;
  }

  return order;
}
