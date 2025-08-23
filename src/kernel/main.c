#include "ordos/kernel/config.h" // IWYU pragma: keep
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem/buddy/buddy.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/common.h"

__asm_linkage void
kmain(void)
{
  init_print();

  kinfo("%s %s", ORDOS_KERNEL_NAME, ORDOS_KERNEL_VERSION);

  init_buddy();

  while (1) {
    hlt();
  }
}
