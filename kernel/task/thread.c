#include "kernel/task/thread.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/config/task.h"
#include "kernel/interrupt/intr.h"
#include "kernel/memory/page.h"
#include "kernel/task/context.h"
#include "kernel/task/task.h"
#include "kernel/types.h"
#include "kernel/utils/list_head.h"
#include "kernel/utils/mm.h"
#include "kernel/utils/print.h"
#include "kernel/utils/string.h"

extern void
thread_switch_to(struct task_t* curr, struct task_t* next);

static struct task_t* main_thread;
static struct list_head* thread_running;
static struct list_head thread_ready_list;
static struct list_head thread_all_list;

static void
kernel_thread(task_function_t function, void* arg)
{
  intr_set_status(true);
  function(arg);
}

static void
init_thread_task(struct task_t* task, char* name, u8 priority)
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
init_thread_stack(struct task_t* task, task_function_t function, void* arg)
{
  task->kstack -= sizeof(struct intr_context_t);
  task->kstack -= sizeof(struct thread_context_t);

  struct thread_context_t* ctx = (struct thread_context_t*)task->kstack;
  ctx->func = function;
  ctx->arg = arg;
  ctx->eip = kernel_thread;

  ctx->ebp = 0;
  ctx->ebx = 0;
  ctx->esi = 0;
  ctx->edi = 0;
}

static void
init_main_thread(void)
{
  main_thread = thread_current();
  init_thread_task(main_thread, "kmain", 31);
  main_thread->status = TASK_STATUS_RUNNING;
  list_add_tail(&main_thread->global_node, &thread_all_list);
}

void
init_thread(void)
{
  list_init(&thread_ready_list);
  list_init(&thread_all_list);

  init_main_thread();
}

struct task_t*
thread_run(char* name, u8 priority, task_function_t function, void* arg)
{
  struct task_t* task = alloc_page(TASK_PCB_PAGE_SIZE);
  init_thread_task(task, name, priority);
  init_thread_stack(task, function, arg);

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
thread_scheduler(void)
{
  KASSERT_MSG(intr_get_status() == false,
              "thread_scheduler: interrupt is enabled");

  struct task_t* curr = thread_current();
  if (curr->status == TASK_STATUS_RUNNING) {
    list_add_tail(&curr->node, &thread_ready_list);
    curr->ticks = curr->priority;
    curr->status = TASK_STATUS_READY;
  }

  KASSERT_MSG(!list_empty(&thread_ready_list),
              "thread_scheduler: no ready thread");
  thread_running = list_pop(&thread_ready_list);
  struct task_t* next = LIST_ENTRY(thread_running, struct task_t, node);
  next->status = TASK_STATUS_RUNNING;

  thread_switch_to(curr, next);
}