#include "kernel/task/task.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/device/sys_clk.h"
#include "kernel/log.h"
#include "kernel/memory/page.h"
#include "kernel/task/context.h"
#include "kernel/task/kthread.h"
#include "kernel/task/tss.h"
#include "kernel/utils/bitmap.h"
#include "kernel/utils/queue.h"
#include "kernel/utils/string.h"

extern void
_asm_thread_switch_to(struct task* curr, struct task* next);

struct atomic_queue_nint task_ready_list;
struct atomic_queue task_all_list;
struct list_head* task_running;

static void
__kernel_task_entry(task_entry_t function, void* arg)
{
  intr_set_status(true);
  function(arg);
}

static void
__task_schedule(void)
{
  KASSERT(intr_get_status() == false, "interrupt is enabled when scheduling");

  struct task* curr = task_current();
  if (curr->status == TASK_STATUS_RUNNING) {
    atomic_queue_nint_push(&task_ready_list, &curr->node);
    curr->ticks = curr->priority;
    curr->status = TASK_STATUS_READY;
  }

  KASSERT(!list_empty(&task_ready_list.head), "no ready thread to schedule");
  task_running = atomic_queue_nint_pop(&task_ready_list);
  struct task* next = LIST_ENTRY(task_running, struct task, node);
  next->status = TASK_STATUS_RUNNING;

  void* page_table = next->page_table != NULL
                       ? PAGE_VADDR_TO_PADDR(next->page_table)
                       : (void*)MEM_PAGE_TABLE_START;
  __asm__ __volatile__("movl %0, %%cr3" : : "r"(page_table) : "memory");

  if (next->page_table != NULL) {
    tss_update_esp(next);
  }

  _asm_thread_switch_to(curr, next);
}

static void
__task_schedule_handler(u32 /* irq */)
{
  struct task* task = task_current();

  KASSERT(task->tmagic == TASK_MAGIC, "invalid task object at %x", task);

  task->elapsed_ticks++;

  if (task->ticks > 0) {
    task->ticks--;
  } else {
    __task_schedule();
  }
}

static void
__init_task_queue(void)
{
  atomic_queue_nint_init(&task_ready_list);
  atomic_queue_init(&task_all_list);
}

void
init_task(void)
{
  KINFO("initializing task management subsystem");

  init_sys_clk();

  __init_task_queue();
  init_kthread();

  init_user_tss();

  intr_register_handler(0x20, __task_schedule_handler);
}

struct task*
task_current(void)
{
  u32 esp;
  __asm__ __volatile__("movl %%esp, %0" : "=g"(esp));
  return (struct task*)(esp & PAGE_SELECTOR_MASK);
}

void
task_init(struct task* task, char* name, u8 priority)
{
  kmemset(task, 0, sizeof(struct task));
  kstrcpy(task->name, name);
  task->status = TASK_STATUS_READY;
  task->priority = priority;
  task->ticks = priority;
  task->elapsed_ticks = 0;
  task->page_table = NULL;
  task->kstack = (u32*)((u32)task + MEM_PAGE_SIZE);
  task->tmagic = TASK_MAGIC;
}

void
task_init_stack(struct task* task, task_entry_t function, void* arg)
{
  task->kstack -= sizeof(struct intr_context);
  task->kstack -= sizeof(struct thread_context);

  struct thread_context* ctx = (struct thread_context*)task->kstack;
  ctx->func = function;
  ctx->arg = arg;
  ctx->eip = __kernel_task_entry;

  ctx->ebp = 0;
  ctx->ebx = 0;
  ctx->esi = 0;
  ctx->edi = 0;
}

void
task_init_vmmap(struct task* task)
{
  size_t bitmap_size =
    (MEM_KERNEL_VSTART - MEM_USER_VSTART) / MEM_PAGE_SIZE / 8;
  size_t bitmap_pages = (bitmap_size + MEM_PAGE_SIZE - 1) / MEM_PAGE_SIZE;

  task->user_vaddr.vstart = MEM_USER_VSTART;
  atomic_bitmap_init(
    &task->user_vaddr.vmmap, alloc_page(bitmap_pages, true), bitmap_size);
}

void
task_init_page_table(struct task* task)
{
  task->page_table = alloc_page(1, true);
  if (task->page_table == NULL) {
    KPANIC("failed to allocate page table for task %s", task->name);
  }

  kmemcpy((void*)((u32)task->page_table + PAGE_PDE_KERNEL_OFFSET),
          (void*)(PAGE_PDE_VSTART + PAGE_PDE_KERNEL_OFFSET),
          MEM_PAGE_SIZE - PAGE_PDE_KERNEL_OFFSET);

  void* paddr = PAGE_VADDR_TO_PADDR(task->page_table);
  task->page_table[PAGE_ENTRIES - 1] = PAGE_PDE_DESC((u32)paddr, 1, 1, 1);
}

void
task_yield(void)
{
  bool intr_status = intr_lock();

  struct task* curr = task_current();

  atomic_queue_nint_push(&task_ready_list, &curr->node);
  curr->status = TASK_STATUS_READY;

  __task_schedule();

  intr_unlock(intr_status);
}

void
task_park(void)
{
  bool intr_status = intr_lock();

  struct task* task = task_current();
  task->status = TASK_STATUS_BLOCKED;

  __task_schedule();

  intr_unlock(intr_status);
}

void
task_unpark(struct task* task)
{
  bool intr_status = intr_lock();

  KASSERT(task->status == TASK_STATUS_BLOCKED ||
            task->status == TASK_STATUS_WAITING ||
            task->status == TASK_STATUS_SUSPENDED,
          "invalid status when resuming, received %u",
          (u32)(task->status));

  if (task->status == TASK_STATUS_READY) {
    return;
  }

  atomic_queue_nint_push(&task_ready_list, &task->node);
  task->status = TASK_STATUS_READY;

  intr_unlock(intr_status);
}