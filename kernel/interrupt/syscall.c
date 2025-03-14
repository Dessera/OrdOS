#include "kernel/interrupt/syscall.h"
#include "kernel/config/interrupt.h"
#include "kernel/log.h"
#include "kernel/task/task.h"
#include "kernel/types.h"

static void* __sysall_table[INTR_SYSCALL_SIZE] = { 0 };

void
init_syscall(void)
{
  __sysall_table[0] = syscall_getpid;
}

size_t
syscall_getpid(void)
{
  return task_current()->pid;
}

void*
syscall_common_handler(u32 index, void* arg1, void* arg2)
{
  void* (*handler)(void*, void*) = __sysall_table[index];
  if (handler != NULL) {
    return handler(arg1, arg2);
  } else {
    KWARNING("unhandled syscall: %u", index);
    return NULL;
  }
}