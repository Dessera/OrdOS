#include "ordos/core/task/kthread.h"
#include "ordos/config.h"
#include "ordos/core/intr.h"
#include "ordos/core/mem.h"
#include "ordos/core/task.h"
#include "ordos/init.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/types.h"

struct task* __kmain = NULL;

struct task* __kidle = NULL;

static void
__task_idle_entry(void* arg)
{
  unused(arg);

  while (true) {
    task_yield();
    intr_set_status(true);
    hlt();
  }
}

int
init_kmain(void)
{
  __kmain = kmalloc(sizeof(struct task));
  if (__kmain == NULL) {
    kwarn("%s: Failed to create kernel main thread", ORDOS_TASK_KMAIN_NAME);
    return E_ALLOC;
  }

  task_init(__kmain, ORDOS_TASK_KMAIN_NAME, ORDOS_TASK_DEFAULT_PRIO);
  __kmain->pd = __init.kernel_pd;
  __kmain->status = TSK_RUNNING;

  task_push_global(__kmain);
  task_set_current(__kmain);

  return E_SUCCESS;
}

int
init_kidle(void)
{
  __kidle = kthread_create(
    ORDOS_TASK_KIDLE_NAME, ORDOS_TASK_IDLE_PRIO, __task_idle_entry, NULL);
  if (__kidle == NULL) {
    return E_ALLOC;
  }

  return E_SUCCESS;
}

struct task*
kthread_create(const char* name,
               task_prio_t prio,
               task_entry_t entry,
               void* arg)
{
  struct task* task = kmalloc(sizeof(struct task));
  if (task == NULL) {
    kwarn("Kthread: Failed to allocate memory for task %s", name);
    return NULL;
  }

  task_init(task, name, prio);
  if (task_init_stack(task, entry, arg) != E_SUCCESS) {
    kfree(task);
    return NULL;
  }

  task->pd = __init.kernel_pd;

  task_push_global(task);
  task_push(task);

  return task;
}
