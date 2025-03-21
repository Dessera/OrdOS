#include "kernel/task/kthread.h"
#include "kernel/log.h"
#include "lib/types.h"

static void
kthread_entry(void* arg)
{
  char* msg = (char*)arg;

  while (true) {
    KINFO("Hello from kthread: %s\n", msg);
  }
}

DECLARE_WITH_PROTOTYPE(void, ktester, void)
{
  kthread_create("1", 31, kthread_entry, "Thread 1");
  kthread_create("2", 31, kthread_entry, "Thread 2");
}