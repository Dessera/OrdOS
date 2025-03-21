#include "kernel/task/task.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/device/clk.h"
#include "kernel/log.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/memory.h"
#include "kernel/task/context.h"
#include "kernel/task/kthread.h"
#include "kernel/task/pid.h"
#include "kernel/task/sync.h"
#include "kernel/utils/list_head.h"
#include "kernel/utils/string.h"
#include "lib/types.h"

extern void
_asm_thread_switch_to(struct task* curr, struct task* next);

struct pidpool __pid_pool;

struct list_head __task_list;
struct list_head __task_ready_list;
struct task* __current_task = NULL;

static void
__task_entry(task_entry_t function, void* arg)
{
  intr_set_status(true);
  function(arg);
}

FORCE_INLINE void
__init_task_queue(void)
{
  list_init(&__task_list);
  list_init(&__task_ready_list);
}

static void
__task_schedule(void)
{
  KASSERT(intr_get_status() == false, "interrupt is enabled when scheduling");

  struct task* curr = task_get_current();
  if (curr->status == TASK_STATUS_RUNNING) {
    list_add_tail(&curr->node, &__task_ready_list);
    curr->ticks = curr->priority;
    curr->status = TASK_STATUS_READY;
  }

  KASSERT(!list_empty(&__task_ready_list), "no ready thread to schedule");
  AUTO next = LIST_ENTRY(list_pop(&__task_ready_list), struct task, node);

  task_set_current(next);
  next->status = TASK_STATUS_RUNNING;

  // void* page_table = next->page_table != NULL
  //                      ? PAGE_VADDR_TO_PADDR(next->page_table)
  //                      : (void*)MEM_PAGE_TABLE_START;
  // __asm__ __volatile__("movl %0, %%cr3" : : "r"(page_table) : "memory");

  // if (next->page_table != NULL) {
  //   tss_update_esp(next);
  // }

  _asm_thread_switch_to(curr, next);
}

static void
__task_schedule_handler(u32 /* irq */)
{
  struct task* task = task_get_current();

  task->elapsed_ticks++;

  if (task->ticks != 0) {
    task->ticks--;
  } else {
    __task_schedule();
  }
}

void
init_task(void)
{
  KINFO("initializing task management subsystem");

  init_clk();

  pidpool_init(&__pid_pool);
  __init_task_queue();

  init_kthread();

  intr_register_handler(0x20, __task_schedule_handler);
}

void
task_init(struct task* task, const char* name, u8 priority)
{
  kstrcpy(task->name, name);
  task->pid = pidpool_alloc(&__pid_pool);
  task->status = TASK_STATUS_READY;
  task->priority = priority;
  task->ticks = priority;
  task->elapsed_ticks = 0;
  task->page_table = NULL;
}

void
task_init_stack(struct task* task, task_entry_t function, void* arg)
{
  task->stack = (void*)(page_get_virt(buddy_alloc_page(MEM_ZONE_NORMAL, 0)) +
                        MEM_PAGE_SIZE);
  task->stack -= sizeof(struct thread_context) + sizeof(struct intr_context);

  struct thread_context* ctx = (struct thread_context*)task->stack;
  ctx->func = function;
  ctx->arg = arg;
  ctx->eip = __task_entry;

  ctx->ebp = 0;
  ctx->ebx = 0;
  ctx->esi = 0;
  ctx->edi = 0;
}

void
task_push(struct task* task)
{
  bool old = intr_lock();
  list_add_tail(&task->node, &__task_ready_list);
  intr_unlock(old);
}

void
task_push_global(struct task* task)
{
  bool old = intr_lock();
  list_add_tail(&task->global_node, &__task_list);
  intr_unlock(old);
}

FORCE_INLINE void
task_set_current(struct task* task)
{
  __current_task = task;
}

FORCE_INLINE struct task*
task_get_current(void)
{
  return __current_task;
}

void
task_yield(void)
{
  bool intr_status = intr_lock();

  struct task* curr = task_get_current();

  task_push(curr);
  curr->status = TASK_STATUS_READY;

  __task_schedule();

  intr_unlock(intr_status);
}

void
task_park(void)
{
  bool intr_status = intr_lock();

  struct task* task = task_get_current();
  task->status = TASK_STATUS_BLOCKED;

  __task_schedule();

  intr_unlock(intr_status);
}

void
task_unpark(struct task* task)
{
  bool intr_status = intr_lock();

  KASSERT(task->status == TASK_STATUS_BLOCKED,
          "invalid status when resuming, received %u",
          (u32)(task->status));

  if (task->status == TASK_STATUS_READY) {
    return;
  }

  task_push(task);
  task->status = TASK_STATUS_READY;

  intr_unlock(intr_status);
}