#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/page.h"
#include "kernel/task/task.h"
#include "kernel/types.h"

void
kmain(void);

static void
kinit(void)
{
  init_vga();

  KINFO("%s STEP 3", KFULLNAME_STR);

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
