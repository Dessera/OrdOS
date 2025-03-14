#include "kernel/device/keyboard.h"
#include "kernel/info.h"
#include "kernel/log.h"

#include "kernel/interrupt/interrupt.h"
#include "kernel/memory/memory.h"
#include "kernel/task/kthread.h"
#include "kernel/task/task.h"
#include "kernel/task/uproc.h"
#include "kernel/utils/print.h"
#include "lib/syscall.h"

size_t pid = 42;

void
kmain(void);

static void
kthread_entry(void*)
{
  while (true) {
    kprintln("uproc: %u", pid);
  }
}

static void
uproc_entry(void)
{
  pid = getpid();

  while (true)
    ;
}

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

  kthread_run("hello_p", 31, kthread_entry, NULL);
  uproc_run("hello", uproc_entry);

  while (true)
    ;
}
