#include "ordos/kernel/task/sync.h"
#include "ordos/lib/common.h"

__inline static i32
__atomic_cmpexchange(volatile i32* ptr, i32 old, i32 new) // NOLINT
{
  i32 ret = 0;
  asm_exec("lock;"
           "cmpxchgl %2, %1;" : "=a"(ret),
           "+m"(*ptr) : "r"(new),
           "0"(old) : "memory");
  return ret;
}

void
spin_lock_init(struct spin_lock* lock)
{
  lock->flag = 0;
}

void
spin_lock(struct spin_lock* lock)
{
  while (__atomic_cmpexchange(&lock->flag, 0, 1) == 1) {
  }
}

void
spin_unlock(struct spin_lock* lock)
{
  lock->flag = 0;
}
