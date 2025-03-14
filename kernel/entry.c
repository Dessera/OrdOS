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
#include "lib/types.h"

void
kmain(void);

static void
uproc_entry(void)
{
  size_t pid = getpid();

  char buf[50] = { 0 };
  utoa(buf, pid, 16);
  buf[1] = '\n';

  while (true) {
    write(buf);
  }
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

  uproc_run("hello", uproc_entry);

  while (true)
    ;
}
