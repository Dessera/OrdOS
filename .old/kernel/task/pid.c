#include "kernel/task/pid.h"
#include "kernel/config/task.h"
#include "kernel/utils/bitmap.h"
#include "kernel/utils/string.h"
#include "lib/types.h"

void
pidpool_init(struct pidpool* pool)
{
  kmemset(pool->buffer, 0, TASK_PID_BITMAP_SIZE);
  atomic_bitmap_init(&pool->pids, pool->buffer, TASK_PID_BITMAP_SIZE);
}

ssize_t
pidpool_alloc(struct pidpool* pool)
{
  return atomic_bitmap_alloc(&pool->pids, 1);
}

void
pidpool_free(struct pidpool* pool, size_t pid)
{
  atomic_bitmap_set(&pool->pids, pid, false);
}