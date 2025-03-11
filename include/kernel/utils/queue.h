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

struct atomic_queue_no_intr
{
  struct list_head head;
};

void
atomic_queue_no_intr_init(struct atomic_queue_no_intr* q);

void
atomic_queue_no_intr_push(struct atomic_queue_no_intr* q,
                          struct list_head* item);

struct list_head*
atomic_queue_no_intr_pop(struct atomic_queue_no_intr* q);
