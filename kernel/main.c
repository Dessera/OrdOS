#include "kernel/info.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/print.h"
#include "lib/types.h"

static void
kinit(void)
{
  init_print();

  KINFO("%s STEP 3", KFULLNAME_STR);

  init_intr();
  init_memory();
}

DECLARE_WITH_PROTOTYPE(void, kmain, void)
{
  kinit();

  while (true)
    ;
}