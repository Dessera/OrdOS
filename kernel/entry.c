#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/memory/memory.h"
#include "kernel/task/sync.h"
#include "kernel/task/task.h"
#include "kernel/task/thread.h"
#include "kernel/types.h"
#include "kernel/utils/print.h"

struct spin_lock_t g_lock;

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
  init_mm();
  init_task();

  spin_lock_init(&g_lock);

  thread_run("thread_a", 16, thread_test, "A A A A A A A A A A A A A A A ");
  thread_run("thread_b", 8, thread_test, "B B B B B B B B B B B B B B B ");
  thread_run("thread_b", 4, thread_test, "C C C C C C C C C C C C C C C ");

  intr_set_status(true);
}

void
kmain(void)
{
  kinit();

  while (true)
    ;
}

void
thread_test(void* arg)
{
  char* str = (char*)arg;

  while (true) {
    spin_lock(&g_lock);
    kputs(str);
    spin_unlock(&g_lock);
  }
}
