#include "kernel/task/pid.h"
#include "kernel/config/task.h"
#include "kernel/task/sync.h"
#include "kernel/utils/bitmap.h"
#include "lib/string.h" // IWYU pragma: keep
#include "lib/types.h"

void
pidpool_init(struct pidpool* pool)
{
  memset(pool->buffer, 0, TASK_PID_BITMAP_SIZE);
  bitmap_init(&pool->pids, pool->buffer, TASK_PID_BITMAP_SIZE);
}

ssize_t
pidpool_alloc(struct pidpool* pool)
{
  spin_lock(&pool->lock);
  AUTO pid = bitmap_alloc(&pool->pids, 1);
  spin_unlock(&pool->lock);

  return pid;
}

void
pidpool_free(struct pidpool* pool, size_t pid)
{
  spin_lock(&pool->lock);
  bitmap_set(&pool->pids, pid, false);
  spin_unlock(&pool->lock);
}