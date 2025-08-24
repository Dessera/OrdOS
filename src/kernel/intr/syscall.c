#include "ordos/kernel/intr/syscall.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/logging.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

static void* __sysall_table[ORDOS_INTR_SYSCALLS_CNT] = {
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

void
init_syscall(void)
{
  kdebug("Syscall: %u handlers", ORDOS_INTR_SYSCALLS_CNT);
}

__asm_linkage void*
syscall_common_handler(size_t index, void* arg1, void* arg2, void* arg3)
{
  if (index >= ORDOS_INTR_SYSCALLS_CNT) {
    kerror_unsafe("Syscall: Invalid index: %u", index);
    return (void*)NPOS;
  }

  syscall_handler_t handler = __sysall_table[index];
  if (handler != NULL) {
    return handler(arg1, arg2, arg3);
  }

  kwarn_unsafe("Syscall: Unhandled syscall: %u", index);
  return (void*)NPOS;
}
