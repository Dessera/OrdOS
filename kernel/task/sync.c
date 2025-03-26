#include "kernel/task/sync.h"
#include "kernel/task/task.h"
#include "lib/types.h"

static u8
__atomic_cmpexchange(volatile i32* ptr, i32 old, i32 new)
{
  u8 ret;

  __asm__ __volatile__("lock;"
                       "cmpxchgl %2, %1;"
                       "sete %0"
                       : "=q"(ret), "=m"(*ptr)
                       : "r"(new), "m"(*ptr), "a"(old)
                       : "memory");
  return ret;
}

void
spin_lock_init(struct spin_lock* lock)
{
  lock->flag = 0;
}

FORCE_INLINE void
spin_lock(struct spin_lock* lock)
{
  while (__atomic_cmpexchange(&lock->flag, 0, 1) == 0)
    ;
}

FORCE_INLINE void
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
    struct task* task = task_get_current();
    list_add_tail(&task->node, &lck->wait_queue);
    spin_unlock(&lck->guard);
    task_park();
  }
}

void
mutex_unlock(struct mutex_lock* lck)
{
  spin_lock(&lck->guard);

  if (list_empty(&lck->wait_queue)) {
    lck->flag = 0;
  } else {
    struct task* task =
      LIST_ENTRY(list_pop(&lck->wait_queue), struct task, node);
    task_unpark(task);
  }

  spin_unlock(&lck->guard);
}

void
semaphore_init(struct semaphore* sem, size_t value)
{
  sem->value = value;
  spin_lock_init(&sem->guard);
  list_init(&sem->wait_queue);
}

void
semaphore_down(struct semaphore* sem)
{
  spin_lock(&sem->guard);

  if (sem->value > 0) {
    // the semaphore is available
    sem->value--;
    spin_unlock(&sem->guard);
  } else {
    // the semaphore is not available
    struct task* task = task_get_current();
    list_add_tail(&task->node, &sem->wait_queue);
    spin_unlock(&sem->guard);
    task_park();
  }
}

void
semaphore_up(struct semaphore* sem)
{
  spin_lock(&sem->guard);

  if (list_empty(&sem->wait_queue)) {
    // no task is waiting
    sem->value++;
  } else {
    // there are tasks waiting
    struct task* task =
      LIST_ENTRY(list_pop(&sem->wait_queue), struct task, node);
    task_unpark(task);
  }

  spin_unlock(&sem->guard);
}

void
semaphore_down_nint(struct semaphore* sem)
{
  bool istate = intr_lock();

  if (sem->value > 0) {
    // the semaphore is available
    sem->value--;
  } else {
    // the semaphore is not available
    struct task* task = task_get_current();
    list_add_tail(&task->node, &sem->wait_queue);
    task_park();
  }

  intr_unlock(istate);
}

void
semaphore_up_nint(struct semaphore* sem)
{
  bool istate = intr_lock();

  if (list_empty(&sem->wait_queue)) {
    // no task is waiting
    sem->value++;
  } else {
    // there are tasks waiting
    struct task* task =
      LIST_ENTRY(list_pop(&sem->wait_queue), struct task, node);
    task_unpark(task);
  }

  intr_unlock(istate);
}