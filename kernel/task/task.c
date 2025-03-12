#include "kernel/task/task.h"
#include "kernel/config/memory.h"
#include "kernel/device/sys_clk.h"
#include "kernel/log.h"
#include "kernel/memory/page.h"
#include "kernel/task/context.h"
#include "kernel/task/thread.h"
#include "kernel/utils/bitmap.h"
#include "kernel/utils/string.h"

static void
__kernel_task_entry(task_function_t function, void* arg)
{
  intr_set_status(true);
  function(arg);
}

void
init_task(void)
{
  KINFO("initializing task management subsystem");

  init_sys_clk();
  init_thread();
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
task_init_stack(struct task* task, task_function_t function, void* arg)
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