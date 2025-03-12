#include "kernel/task/thread.h"
#include "kernel/assert.h"
#include "kernel/config/task.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/memory/page.h"
#include "kernel/task/sync.h"
#include "kernel/task/task.h"
#include "kernel/types.h"
#include "kernel/utils/list_head.h"
#include "kernel/utils/queue.h"

extern void
_asm_thread_switch_to(struct task* curr, struct task* next);

static struct task* main_thread;
static struct list_head* thread_running;
static struct atomic_queue_nint thread_ready_list;
static struct atomic_queue thread_all_list;

static void
__thread_schedule_handler(u32 irq)
{
  (void)irq;

  struct task* task = task_current();

  KASSERT(task->tmagic == TASK_MAGIC, "invalid task object at %x", task);

  task->elapsed_ticks++;

  if (task->ticks > 0) {
    task->ticks--;
  } else {
    thread_schedule();
  }
}

static void
__init_thread_queue(void)
{
  atomic_queue_nint_init(&thread_ready_list);
  atomic_queue_init(&thread_all_list);
}

static void
__init_main_thread(void)
{
  main_thread = task_current();
  task_init(main_thread, "kmain", 31);
  main_thread->status = TASK_STATUS_RUNNING;

  atomic_queue_push(&thread_all_list, &main_thread->global_node);
  thread_running = &main_thread->node;
}

void
init_thread(void)
{
  __init_thread_queue();
  __init_main_thread();

  intr_register_handler(0x20, __thread_schedule_handler);
}

struct task*
thread_run(char* name, u8 priority, task_function_t function, void* arg)
{
  struct task* task = alloc_page(TASK_PCB_PAGE_SIZE, true);
  task_init(task, name, priority);
  task_init_stack(task, function, arg);

  atomic_queue_nint_push(&thread_ready_list, &task->node);
  atomic_queue_push(&thread_all_list, &task->global_node);

  return task;
}

void
thread_schedule(void)
{
  KASSERT(intr_get_status() == false, "interrupt is enabled when scheduling");

  struct task* curr = task_current();
  if (curr->status == TASK_STATUS_RUNNING) {
    atomic_queue_nint_push(&thread_ready_list, &curr->node);
    curr->ticks = curr->priority;
    curr->status = TASK_STATUS_READY;
  }

  KASSERT(!list_empty(&thread_ready_list.head), "no ready thread to schedule");
  thread_running = atomic_queue_nint_pop(&thread_ready_list);
  struct task* next = LIST_ENTRY(thread_running, struct task, node);
  next->status = TASK_STATUS_RUNNING;

  _asm_thread_switch_to(curr, next);
}

void
thread_yield(void)
{
  bool intr_status = intr_lock();

  struct task* curr = task_current();

  KASSERT(!list_find(&thread_ready_list.head, &curr->node),
          "thread %s cannot yield because it is not running",
          curr->name);
  atomic_queue_nint_push(&thread_ready_list, &curr->node);
  curr->status = TASK_STATUS_READY;

  thread_schedule();

  intr_unlock(intr_status);
}

void
thread_park(void)
{
  bool intr_status = intr_lock();

  struct task* task = task_current();
  task->status = TASK_STATUS_BLOCKED;

  thread_schedule();

  intr_unlock(intr_status);
}

void
thread_unpark(struct task* task)
{
  bool old_intr_status = intr_lock();

  KASSERT(task->status == TASK_STATUS_BLOCKED ||
            task->status == TASK_STATUS_WAITING ||
            task->status == TASK_STATUS_SUSPENDED,
          "invalid status when resuming, received %u",
          (u32)(task->status));

  // if (task->status == TASK_STATUS_READY) {
  //   return;
  // }

  KASSERT(list_find(&thread_ready_list.head, &task->node) == -1,
          "task %x is already in ready list",
          task);

  atomic_queue_nint_push(&thread_ready_list, &task->node);
  task->status = TASK_STATUS_READY;

  intr_unlock(old_intr_status);
}