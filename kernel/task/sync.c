#include "kernel/task/sync.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/task/task.h"
#include "kernel/task/thread.h"
#include "kernel/types.h"
#include "kernel/utils/list_head.h"

void
spin_lock_init(struct spin_lock* lock)
{
  lock->flag = 0;
}

void
spin_lock(struct spin_lock* lock)
{
  while (atomic_cmpexchange(&lock->flag, 0, 1) == 1)
    ;
}

void
spin_unlock(struct spin_lock* lock)
{
  lock->flag = 0;
}

void
mutex_lock_init(struct mutex_lock* lck)
{
  lck->flag = 0;
  spin_lock_init(&lck->guard);
  list_init(&lck->wait_queue);
}

void
mutex_lock(struct mutex_lock* lck)
{
  spin_lock(&lck->guard);
  if (lck->flag == 0) {
    lck->flag = 1;
    spin_unlock(&lck->guard);
  } else {
    struct task_t* task = thread_current();
    list_add_tail(&task->node, &lck->wait_queue);
    spin_unlock(&lck->guard);
    thread_park();
  }
}

void
mutex_unlock(struct mutex_lock* lck)
{
  spin_lock(&lck->guard);

  if (list_empty(&lck->wait_queue)) {
    lck->flag = 0;
  } else {
    struct task_t* task =
      LIST_ENTRY(list_pop(&lck->wait_queue), struct task_t, node);
    thread_unpark(task);
  }

  spin_unlock(&lck->guard);
}

bool
intr_lock(void)
{
  return intr_set_status(false);
}

void
intr_unlock(bool flag)
{
  intr_set_status(flag);
}