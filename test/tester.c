#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/task/task.h"
#include "kernel/utils/print.h"

extern void
ktester(void);

static void
kinit(void)
{
  init_print();
  init_intr();
  init_memory();
  init_task();

  intr_set_status(true);
}

DECLARE_WITH_PROTOTYPE(void, kmain, void)
{
  kinit();

  KINFO("running tests");

  ktester();

  KINFO("all tests passed");

  while (true)
    ;
}