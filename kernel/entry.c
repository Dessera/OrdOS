#include "kernel/device/sys_clk.h"
#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/intr.h"
#include "kernel/memory/mm.h"
#include "kernel/task/thread.h"
#include "kernel/types.h"
#include "kernel/utils/print.h"

void
kmain(void);

void
thread_test(void* arg);

static void
kinit(void)
{
  init_vga();

  kputs(KFULLNAME_STR);
  kputs(" STEP 3\n");

  init_intr();
  init_sys_clk();
  init_mm();
  init_thread();

  thread_run("t1", 31, thread_test, " OA ");
  thread_run("t2", 31, thread_test, " OB ");

  intr_set_status(true);
}

void
kmain(void)
{
  kinit();

  // thread_run("thread_test", 31, thread_test, " OA ");
  // thread_run("thread_test", 31, thread_test, " OB ");

  while (true)
    ;
}

void
thread_test(void* arg)
{
  char* str = (char*)arg;

  while (true) {
    kputs(str);
  }
}