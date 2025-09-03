#include "ordos/core/task.h"
#include "ordos/config.h"
#include "ordos/core/mem.h"
#include "ordos/core/mem/buddy.h"
#include "ordos/core/mem/buddy/page.h"
#include "ordos/core/task/context.h"
#include "ordos/core/task/kthread.h"
#include "ordos/core/task/pid.h"
#include "ordos/core/task/sched.h"
#include "ordos/core/task/tss.h"
#include "ordos/drv/pit.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/sync.h"
#include "ordos/lib/types.h"
#include "ordos/lib/util/list_head.h"
#include "ordos/module.h"

static struct pidpool __pids;

static struct list_head __tasks_list;
static struct list_head __tasks_ready_list;

static struct task* __task_curr = NULL;

static struct sched* __task_sched = NULL;

static void
__task_entry(task_entry_t function, void* arg)
{
  intr_set_status(true);
  function(arg);
}

static void
__task_pit_handler(size_t ticks)
{
  unused(ticks);
  __task_sched->entry(&__tasks_ready_list);
}

static void
__task_schedule(void)
{
  __task_sched->fentry(&__tasks_ready_list);
}

void
task_init(struct task* task, const char* name, task_prio_t priority)
{
  task->pid = pidpool_alloc(&__pids);
  strncpy(task->name, name, ORDOS_TASK_NAME_LENGTH);
  task->prio = priority;
  task->status = TSK_READY;
  task->ticks = 0;
  task->vticks = 0;
  task->pd = NULL;
  task->stack = NULL;
}

int
task_init_stack(struct task* task, task_entry_t function, void* arg)
{
  struct page* page = buddy_alloc_page(MEM_NORMAL, 0);
  if (page == NULL) {
    kwarn("task: failed to allocate stack for task %s", task->name);
    return E_ALLOC;
  }

  memset((void*)page_get_virt(page), 0, ORDOS_KERNEL_PAGE_SIZE);

  task->stack = (void*)(page_get_virt(page) + ORDOS_KERNEL_PAGE_SIZE);
  task->stack -= sizeof(struct thread_context) + sizeof(struct intr_context);

  struct thread_context* ctx = (struct thread_context*)task->stack;
  ctx->func = function;
  ctx->arg = arg;
  ctx->eip = __task_entry;

  ctx->ebp = 0;
  ctx->ebx = 0;
  ctx->esi = 0;
  ctx->edi = 0;

  return E_SUCCESS;
}

void
task_push(struct task* task)
{
  bool old = intr_lock();
  list_add_tail(&task->node, &__tasks_ready_list);
  intr_unlock(old);
}

void
task_push_global(struct task* task)
{
  bool old = intr_lock();
  list_add_tail(&task->global_node, &__tasks_list);
  intr_unlock(old);
}

void
task_set_current(struct task* task)
{
  __task_curr = task;
}

struct task*
task_get_current(void)
{
  return __task_curr;
}

void
task_yield(void)
{
  bool intr_status = intr_lock();

  struct task* curr = task_get_current();

  task_push(curr);
  curr->status = TSK_READY;

  __task_schedule();

  intr_unlock(intr_status);
}

void
task_park(void)
{
  bool intr_status = intr_lock();

  struct task* task = task_get_current();
  task->status = TSK_BLOCKED;

  __task_schedule();

  intr_unlock(intr_status);
}

void
task_unpark(struct task* task)
{
  bool intr_status = intr_lock();

  if (task->status == TSK_READY) {
    return;
  }

  task_push(task);
  task->status = TSK_READY;

  intr_unlock(intr_status);
}

int
task_entry(struct module* mod)
{
  pidpool_init(&__pids);

  list_init(&__tasks_list);
  list_init(&__tasks_ready_list);

  if (init_kmain() != E_SUCCESS) {
    merror(mod, "Failed to create kernel main thread");
    return E_KERNPANIC;
  }

  if (init_kidle() != E_SUCCESS) {
    merror(mod, "Failed to create kernel idle thread");
    return E_KERNPANIC;
  }

  init_tss();

  size_t scheds = autoload_module(MOD_SCHED | MOD_AUTOLOAD);
  minfo(mod, "%u schedulers", scheds);

  // TODO: Mutable
  __task_sched = sched_find("rr");
  if (__task_sched == NULL) {
    merror(mod, "Failed to load scheduler rr");
  }

  pit_register(1, __task_pit_handler);

  return E_SUCCESS;
}

module_dependency(sys_mem);
module_dependency(drv_pit);

module_init_noexit(sys_task, MOD_CORE, task_entry, sys_mem, drv_pit);
