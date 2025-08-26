#include "ordos/kernel/config.h" // IWYU pragma: keep
#include "ordos/kernel/intr/intr.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem/memory.h"
#include "ordos/kernel/module.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

__asm_linkage __noreturn void
kmain(void)
{
  init_print();

  kinfo("%s %s", ORDOS_KERNEL_NAME, ORDOS_KERNEL_VERSION);

  init_intr();
  init_memory();

  init_module();

  while (true) {
    hlt();
  }
}
