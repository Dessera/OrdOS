#include "kernel/device/keyboard.h"
#include "kernel/info.h"
#include "kernel/log.h"

#include "kernel/interrupt/interrupt.h"
#include "kernel/memory/memory.h"
#include "kernel/task/task.h"
#include "kernel/utils/print.h"

void
kmain(void);

static void
kinit(void)
{
  init_print();

  KINFO("%s STEP 3", KFULLNAME_STR);

  init_intr();
  init_mm();
  init_task();

  init_keyboard();

  intr_set_status(true);
}

void
kmain(void)
{
  kinit();

  while (true)
    ;
}
