#include "kernel/interrupt/syscall.h"
#include "kernel/assert.h"
#include "kernel/config/interrupt.h"
#include "kernel/log.h"
#include "kernel/task/task.h"
#include "lib/string.h"  // IWYU pragma: keep
#include "lib/syscall.h" // IWYU pragma: keep
#include "lib/types.h"

static void* __sysall_table[INTR_SYSCALL_SIZE] = {
  sys_getpid, sys_write, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr,    nullptr,   nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};

size_t
sys_getpid(void)
{
  return task_get_current()->pid;
}

// TODO: implement a real file system
size_t
sys_write(size_t, const void* buf, size_t)
{
  kputs_nint(buf);
  return strlen(buf);
}

void
init_syscall(void)
{
  KASSERT(INTR_SYSCALL_SIZE >= intr_syscall_get_size(),
          "syscall table overflow");
  KDEBUG("syscall: %u", intr_syscall_get_size());
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
    return (void*)NPOS;
  }
  syscall_handler_t handler = __sysall_table[index];
  if (handler != nullptr) {
    return handler(arg1, arg2, arg3);
  } else {
    KWARNING_NINT("unhandled syscall: %u", index);
    return (void*)NPOS;
  }
}