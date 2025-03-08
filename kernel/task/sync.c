#include "kernel/task/sync.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/types.h"

void
spin_lock_init(struct spin_lock_t* lock)
{
  lock->flag = 0;
}

void
spin_lock(struct spin_lock_t* lock)
{
  while (atomic_cmpexchange(&lock->flag, 0, 1) == 1)
    ;
}

void
spin_unlock(struct spin_lock_t* lock)
{
  lock->flag = 0;
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