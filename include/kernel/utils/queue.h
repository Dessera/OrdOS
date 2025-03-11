#pragma once

#include "kernel/task/sync.h"
#include "kernel/utils/list_head.h"

struct atomic_queue
{
  struct list_head head;
  struct mutex_lock lock;
};

void
atomic_queue_init(struct atomic_queue* q);

void
atomic_queue_push(struct atomic_queue* q, struct list_head* item);

struct list_head*
atomic_queue_pop(struct atomic_queue* q);

/**
 * @brief Queue which blocks interrupts while accessing it, used for resources
 * used both outside and inside the interrupt context.
 */
struct atomic_queue_nint
{
  struct list_head head;
};

void
atomic_queue_nint_init(struct atomic_queue_nint* q);

void
atomic_queue_nint_push(struct atomic_queue_nint* q, struct list_head* item);

struct list_head*
atomic_queue_nint_pop(struct atomic_queue_nint* q);
