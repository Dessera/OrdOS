#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/task/task.h"
#include "kernel/types.h"
#include "kernel/utils/print.h"

void
kmain(void);

static void
kinit(void)
{
  init_vga();

  kputs(KFULLNAME_STR);
  kputs(" STEP 3\n");

  init_intr();
  init_mm();
  init_task();

  intr_set_status(true);
}

void
kmain(void)
{
  kinit();

  KTRACE("Kernel main function %d", 42);
  KDEBUG("Kernel main function %d", 42);
  KINFO("Kernel main function %d", 42);
  KWARNING("Kernel main function %d", 42);
  KERROR("Kernel main function %d", 42);

  while (true)
    ;
}
