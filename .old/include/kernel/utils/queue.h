#pragma once

#include "kernel/task/sync.h"
#include "kernel/utils/list_head.h"

/**
 * @brief Queue with mutex lock
 */
struct atomic_queue
{
  struct list_head head;
  struct mutex_lock lock;
};

/**
 * @brief Initialize the queue
 *
 * @param q The queue to initialize
 */
void
atomic_queue_init(struct atomic_queue* q);

/**
 * @brief Push an item to the queue
 *
 * @param q The queue to push the item to
 * @param item The item to push
 */
void
atomic_queue_push(struct atomic_queue* q, struct list_head* item);

/**
 * @brief Pop an item from the queue
 *
 * @param q The queue to pop the item from
 * @return struct list_head* The item that was popped
 */
struct list_head*
atomic_queue_pop(struct atomic_queue* q);

/**
 * @brief Queue which blocks interrupts while accessing it, used for resources
 * used both outside and inside the interrupt context
 */
struct atomic_queue_nint
{
  struct list_head head;
};

/**
 * @brief Initialize the queue
 *
 * @param q The queue to initialize
 */
void
atomic_queue_nint_init(struct atomic_queue_nint* q);

/**
 * @brief Push an item to the queue
 *
 * @param q The queue to push the item to
 * @param item The item to push
 */
void
atomic_queue_nint_push(struct atomic_queue_nint* q, struct list_head* item);

/**
 * @brief Pop an item from the queue
 *
 * @param q The queue to pop the item from
 * @return struct list_head* The item that was popped
 */
struct list_head*
atomic_queue_nint_pop(struct atomic_queue_nint* q);
