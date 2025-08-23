#include "ordos/kernel/logging.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/common.h"

__asm_linkage void
kmain(void)
{
  init_print();

  kputs_unsafe("HELLO!");

  while (1) {
    hlt();
  }
}
