#include "ordos/config.h" // IWYU pragma: keep
#include "ordos/lib/common.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/module.h"

__asm_linkage __noreturn void
kmain(void)
{
  init_module();

  if (load_module("sys_output") != E_SUCCESS) {
    kpanic_prelude("Failed to load basic output system");
  }

  kinfo("%s %s", ORDOS_KERNEL_NAME, ORDOS_KERNEL_VERSION);

  autoload_module(MOD_CORE);

  while (true) {
    hlt();
  }
}
