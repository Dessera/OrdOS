#include "kernel/task/task.h"
#include "kernel/device/sys_clk.h"
#include "kernel/log.h"
#include "kernel/task/thread.h"
#include "kernel/utils/print.h"
void
init_task(void)
{
  KINFO("initializing task management subsystem");

  init_sys_clk();
  init_thread();
}