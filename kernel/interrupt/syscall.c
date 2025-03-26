#include "kernel/interrupt/syscall.h"
#include "kernel/config/interrupt.h"
#include "kernel/log.h"
#include "kernel/task/task.h"
#include "kernel/utils/string.h"
#include "lib/syscall.h"
#include "lib/types.h"

static void* __sysall_table[INTR_SYSCALL_SIZE] = { 0 };

static size_t
sys_getpid(void)
{
  return task_get_current()->pid;
}

// TODO: implement a real file system
static size_t
sys_write(size_t, const char* buf, size_t)
{
  kputs_nint(buf);
  return kstrlen(buf);
}

void
init_syscall(void)
{
  __sysall_table[SYSCALL_GETPID] = sys_getpid;
  __sysall_table[SYSCALL_WRITE] = sys_write;

  KDEBUG("syscall: %u", INTR_SYSCALL_SIZE);
}

DECLARE_WITH_PROTOTYPE(void*,
                       syscall_common_handler,
                       u32 index,
                       void* arg1,
                       void* arg2,
                       void* arg3)
{
  if (index >= INTR_SYSCALL_SIZE) {
    KERROR_NINT("syscall: invalid index: %u", index);
    return NULL;
  }
  syscall_handler_t handler = __sysall_table[index];
  if (handler != NULL) {
    return handler(arg1, arg2, arg3);
  } else {
    KWARNING_NINT("unhandled syscall: %u", index);
    return NULL;
  }
}