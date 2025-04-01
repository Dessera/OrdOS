#include "kernel/task/task.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/config/task.h"
#include "kernel/device/clk.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/log.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/vpage.h"
#include "kernel/task/context.h"
#include "kernel/task/kthread.h"
#include "kernel/task/pid.h"
#include "kernel/task/sync.h"
#include "kernel/task/tss.h"
#include "lib/list_head.h"
#include "lib/string.h" // IWYU pragma: keep
#include "lib/types.h"

extern void
_asm_task_switch_to(struct task* curr, struct task* next);

static struct pidpool __pid_pool;

static struct list_head __task_list;
static struct list_head __task_ready_list;
static struct task* __current_task = nullptr;

static size_t __ticks = 0;

static void
__task_entry(task_entry_t function, void* arg)
{
  intr_set_status(true);
  function(arg);
}

static FORCE_INLINE void
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

  if (list_empty(&__task_ready_list)) {
    task_unpark(kthread_get_idle());
  }

  AUTO next = LIST_ENTRY(list_pop(&__task_ready_list), struct task, node);

  task_set_current(next);
  next->status = TASK_STATUS_RUNNING;

  __asm__ __volatile__("movl %0, %%cr3" : : "r"(next->page_table) : "memory");
  tss_update_esp(next);

  _asm_task_switch_to(curr, next);
}

static void
__task_schedule_handler(u32 /* irq */)
{
  struct task* task = task_get_current();

  // update ticks
  __ticks++;

  task->elapsed_ticks++;

  if (task->ticks != 0) {
    task->ticks--;
  } else {
    __task_schedule();
  }
}

static void
__task_init_fds(struct task* task)
{
  for (i32 i = 0; i < 3; i++) {
    task->fds[i] = i;
  }

  for (i32 i = 3; i < TASK_FD_SIZE; i++) {
    task->fds[i] = -1;
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
  init_tss();

  intr_register_handler(0x20, __task_schedule_handler);
}

void
task_init(struct task* task, const char* name, u8 priority)
{
  // TODO: maybe truncate name?
  KASSERT(strlen(name) < TASK_NAME_SIZE, "task name is too long");

  strcpy(task->name, name);
  task->pid = pidpool_alloc(&__pid_pool);
  task->status = TASK_STATUS_READY;
  task->priority = priority;
  task->ticks = priority;
  task->elapsed_ticks = 0;
  task->page_table = 0;
  __task_init_fds(task);
}

bool
task_init_stack(struct task* task, task_entry_t function, void* arg)
{
  AUTO page = buddy_alloc_page(MEM_ZONE_NORMAL, 0);
  if (page == nullptr) {
    KWARNING("failed to allocate stack for task %s", task->name);
    return false;
  }

  memset((void*)page_get_virt(page), 0, MEM_PAGE_SIZE);

  task->stack = (void*)(page_get_virt(page) + MEM_PAGE_SIZE);
  task->stack -= sizeof(struct thread_context) + sizeof(struct intr_context);

  struct thread_context* ctx = (struct thread_context*)task->stack;
  ctx->func = function;
  ctx->arg = arg;
  ctx->eip = __task_entry;

  ctx->ebp = 0;
  ctx->ebx = 0;
  ctx->esi = 0;
  ctx->edi = 0;

  return true;
}

bool
task_init_page_table(struct task* task)
{
  AUTO page = buddy_alloc_page(MEM_ZONE_NORMAL, 0);

  if (page == nullptr) {
    KWARNING("failed to allocate page table for task %s", task->name);
    return false;
  }

  memset((void*)page_get_virt(page), 0, MEM_PAGE_SIZE);

  task->page_table = page_get_phys(page);

  // copy kernel page table
  memcpy((void*)(page_get_virt(page) + PAGE_PDE_KERNEL_OFFSET * PAGE_DESC_SIZE),
         (void*)((uintptr_t)vpage_kernel_vaddr() +
                 PAGE_PDE_KERNEL_OFFSET * PAGE_DESC_SIZE),
         (MEM_PAGE_SIZE - PAGE_PDE_KERNEL_OFFSET * PAGE_DESC_SIZE));

  return true;
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
  KASSERT(__current_task != nullptr, "unable to get current task (null)");
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

  if (task->status == TASK_STATUS_READY) {
    return;
  }

  task_push(task);
  task->status = TASK_STATUS_READY;

  intr_unlock(intr_status);
}

void
task_sleep_ticks(size_t ticks)
{
  size_t now = __ticks;
  size_t end = now + ticks;

  while (true) {
    if (end < now) {
      // there is overflow
      if (__ticks < now && __ticks >= end) {
        break;
      }
    } else {
      if (__ticks >= end) {
        break;
      }
    }

    task_yield();
  }
}