#include "kernel/task/thread.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/config/task.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/memory/page.h"
#include "kernel/task/context.h"
#include "kernel/task/sync.h"
#include "kernel/task/task.h"
#include "kernel/types.h"
#include "kernel/utils/list_head.h"
#include "kernel/utils/string.h"

extern void
_asm_thread_switch_to(struct task_t* curr, struct task_t* next);

static struct task_t* main_thread;
static struct list_head* thread_running;
static struct list_head thread_ready_list;
static struct list_head thread_all_list;

static void
__kernel_thread(task_function_t function, void* arg)
{
  intr_set_status(true);
  function(arg);
}

static void
__thread_schedule_handler(u32 irq)
{
  (void)irq;

  struct task_t* task = thread_current();

  KASSERT(task->tmagic == TASK_MAGIC, "invalid task object at %x", task);

  task->elapsed_ticks++;

  if (task->ticks > 0) {
    task->ticks--;
  } else {
    thread_schedule();
  }
}

static void
__thread_init_task(struct task_t* task, char* name, u8 priority)
{
  kmemset(task, 0, sizeof(struct task_t));
  kstrcpy(task->name, name);
  task->status = TASK_STATUS_READY;
  task->priority = priority;
  task->ticks = priority;
  task->elapsed_ticks = 0;
  task->page_table = NULL;
  task->kstack = (u32*)((u32)task + MEM_PAGE_SIZE);
  task->tmagic = TASK_MAGIC;
}

static void
__thread_init_stack(struct task_t* task, task_function_t function, void* arg)
{
  task->kstack -= sizeof(struct intr_context_t);
  task->kstack -= sizeof(struct thread_context_t);

  struct thread_context_t* ctx = (struct thread_context_t*)task->kstack;
  ctx->func = function;
  ctx->arg = arg;
  ctx->eip = __kernel_thread;

  ctx->ebp = 0;
  ctx->ebx = 0;
  ctx->esi = 0;
  ctx->edi = 0;
}

static void
__init_thread_queue(void)
{
  list_init(&thread_ready_list);
  list_init(&thread_all_list);
}

static void
__init_main_thread(void)
{
  main_thread = thread_current();
  __thread_init_task(main_thread, "kmain", 31);
  main_thread->status = TASK_STATUS_RUNNING;

  list_add_tail(&main_thread->global_node, &thread_all_list);
  thread_running = &main_thread->node;
}

void
init_thread(void)
{
  __init_thread_queue();
  __init_main_thread();

  intr_register_handler(0x20, __thread_schedule_handler);
}

struct task_t*
thread_run(char* name, u8 priority, task_function_t function, void* arg)
{
  struct task_t* task = alloc_page(TASK_PCB_PAGE_SIZE);
  __thread_init_task(task, name, priority);
  __thread_init_stack(task, function, arg);

  list_add_tail(&task->global_node, &thread_all_list);
  list_add_tail(&task->node, &thread_ready_list);

  return task;
}

struct task_t*
thread_current(void)
{
  u32 esp;
  __asm__ __volatile__("movl %%esp, %0" : "=g"(esp));
  return (struct task_t*)(esp & PAGE_SELECTOR_MASK);
}

void
thread_schedule(void)
{
  KASSERT(intr_get_status() == false, "interrupt is enabled when scheduling");

  struct task_t* curr = thread_current();
  if (curr->status == TASK_STATUS_RUNNING) {
    list_add_tail(&curr->node, &thread_ready_list);
    curr->ticks = curr->priority;
    curr->status = TASK_STATUS_READY;
  }

  KASSERT(!list_empty(&thread_ready_list), "no ready thread to schedule");
  thread_running = list_pop(&thread_ready_list);
  struct task_t* next = LIST_ENTRY(thread_running, struct task_t, node);
  next->status = TASK_STATUS_RUNNING;

  _asm_thread_switch_to(curr, next);
}

void
thread_yield(void)
{
  bool intr_status = intr_lock();

  struct task_t* curr = thread_current();

  KASSERT(!list_find(&thread_ready_list, &curr->node),
          "thread %s cannot yield because it is not running",
          curr->name);
  list_add_tail(&curr->node, &thread_ready_list);
  curr->status = TASK_STATUS_READY;

  thread_schedule();

  intr_unlock(intr_status);
}

void
thread_park(void)
{
  bool intr_status = intr_lock();

  struct task_t* task = thread_current();
  task->status = TASK_STATUS_BLOCKED;

  thread_schedule();

  intr_unlock(intr_status);
}

void
thread_unpark(struct task_t* task)
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

  KASSERT(list_find(&thread_ready_list, &task->node) == -1,
          "task %x is already in ready list",
          task);

  list_add_tail(&task->node, &thread_ready_list);
  task->status = TASK_STATUS_READY;

  intr_unlock(old_intr_status);
}