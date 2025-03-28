#include "kernel/task/kthread.h"
#include "kernel/config/task.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/vpage.h"
#include "kernel/task/task.h"
#include "lib/asm.h"

static struct task* __kmain_task;
static struct task* __idle_task;

static void
__task_idle_entry(void*)
{
  while (true) {
    task_park();
    intr_set_status(true);
    hlt();
  }
}

static void
__inin_kmain(void)
{
  __kmain_task = kmalloc(sizeof(struct task));
  if (__kmain_task == nullptr) {
    KPANIC("failed to allocate memory for kmain task");
  }

  task_init(__kmain_task, TASK_KTHREAD_MAIN_NAME, TASK_DEFAULT_PRIORITY);
  __kmain_task->page_table = vpage_kernel_paddr();
  __kmain_task->status = TASK_STATUS_RUNNING;

  task_push_global(__kmain_task);
  task_set_current(__kmain_task);
}

static void
__init_idle(void)
{
  __idle_task = kthread_create(
    TASK_IDLE_NAME, TASK_IDLE_PRIORITY, __task_idle_entry, nullptr);
  if (__idle_task == nullptr) {
    KPANIC("failed to create idle task");
  }
}

void
init_kthread(void)
{
  __inin_kmain();
  __init_idle();
}

struct task*
kthread_create(char* name, u8 priority, task_entry_t function, void* arg)
{
  // create a new task
  struct task* task = kmalloc(sizeof(struct task));
  if (!task) {
    KWARNING("failed to allocate memory for task %s", name);
    return nullptr;
  }

  // initialize the task
  task_init(task, name, priority);

  // initialize the task stack
  bool res = task_init_stack(task, function, arg);
  if (!res) {
    kfree(task);
    return nullptr;
  }

  // set the page table (kernel space)
  task->page_table = vpage_kernel_paddr();

  // push the task to the task list
  task_push_global(task);
  task_push(task);

  return task;
}

struct task*
kthread_get_idle(void)
{
  return __idle_task;
}