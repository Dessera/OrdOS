#include "kernel/task/task.h"
#include "kernel/device/sys_clk.h"
#include "kernel/task/thread.h"
#include "kernel/utils/print.h"
void
init_task(void)
{
  kputs("Initializing task scheduler...\n");

  init_sys_clk();
  init_thread();
}