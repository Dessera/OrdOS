#include "kernel/interrupt/syscall.h"
#include "kernel/config/interrupt.h"
#include "kernel/log.h"
#include "kernel/task/task.h"
#include "lib/types.h"
#include "user/syscall.h"

static void* __sysall_table[INTR_SYSCALL_SIZE] = { 0 };

void
init_syscall(void)
{
  __sysall_table[SYSCALL_GETPID] = syscall_getpid;
}

size_t
syscall_getpid(void)
{
  return task_current()->pid;
}

void*
syscall_common_handler(u32 index, void* arg1, void* arg2, void* arg3)
{
  syscall_handler_t handler = __sysall_table[index];
  if (handler != NULL) {
    return handler(arg1, arg2, arg3);
  } else {
    KWARNING("unhandled syscall: %u", index);
    return (void*)NPOS;
  }
}