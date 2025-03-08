#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/memory/memory.h"
#include "kernel/task/sync.h"
#include "kernel/task/task.h"
#include "kernel/types.h"
#include "kernel/utils/print.h"

struct spin_lock_t g_lock;

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

  while (true)
    ;
}
