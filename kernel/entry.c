#include "kernel/info.h"
#include "kernel/log.h"
#include "kernel/task/thread.h"
#include "kernel/types.h"

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

  intr_set_status(true);
}

static void
kthread(void* arg)
{
  char* msg = (char*)arg;
  while (true) {
    kprintln("%s", msg);
  }
}

void
kmain(void)
{
  kinit();

  thread_run("Ath", 31, kthread, "Hello, World!");
  thread_run("Bth", 16, kthread, "Goodbye, World!");

  while (true)
    ;
}
