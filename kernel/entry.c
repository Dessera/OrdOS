#include "kernel/device/keyboard.h"
#include "kernel/info.h"
#include "kernel/log.h"

#include "kernel/interrupt/interrupt.h"
#include "kernel/memory/memory.h"
#include "kernel/task/kthread.h"
#include "kernel/task/task.h"
#include "kernel/task/uproc.h"
#include "kernel/utils/print.h"

void
kmain(void);

// static u32 cnt = 0;

// static void
// uprog(void)
// {
//   while (true) {
//     // safe add cnt
//     cnt++;
//   }
// }

// static void
// kthread(void*)
// {
//   while (true) {
//     KINFO("cnt = %u", cnt);
//   }
// }

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

  // kthread_run("comsumer", 31, kthread, NULL);
  // uproc_run("generator", uprog);

  while (true)
    ;
}
