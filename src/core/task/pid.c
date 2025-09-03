#include "ordos/core/task/pid.h"
#include "ordos/config.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

void
pidpool_init(struct pidpool* pool)
{
  memset(pool->buffer, 0, ORDOS_TASK_PIDPOOL_SIZE);
  bitmap_init(&pool->pids, pool->buffer, ORDOS_TASK_PIDPOOL_SIZE);
}

ssize_t
pidpool_alloc(struct pidpool* pool)
{
  spin_lock(&pool->lock);
  ssize_t pid = bitmap_alloc(&pool->pids, 1);
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
