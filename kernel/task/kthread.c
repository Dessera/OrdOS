#include "kernel/task/kthread.h"
#include "kernel/config/task.h"
#include "kernel/memory/memory.h"
#include "kernel/task/task.h"
#include "kernel/utils/print.h"

static struct task* __kmain_task;

static void
__inin_kmain(void)
{
  __kmain_task = kmalloc(sizeof(struct task));
  task_init(__kmain_task, TASK_KTHREAD_MAIN_NAME, TASK_DEFAULT_PRIORITY);
  __kmain_task->status = TASK_STATUS_RUNNING;

  task_push_global(__kmain_task);
  task_set_current(__kmain_task);
}

void
init_kthread(void)
{
  __inin_kmain();
}

struct task*
kthread_create(char* name, u8 priority, task_entry_t function, void* arg)
{
  struct task* task = kmalloc(sizeof(struct task));

  task_init(task, name, priority);
  task_init_stack(task, function, arg);

  task_push_global(task);
  task_push(task);

  return task;
}