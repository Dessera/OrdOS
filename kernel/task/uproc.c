#include "kernel/task/uproc.h"
#include "kernel/config/memory.h"
#include "kernel/config/task.h"
#include "kernel/log.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/gdt.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/vpage.h"
#include "kernel/task/context.h"
#include "kernel/task/task.h"
#include "lib/types.h"

static FORCE_INLINE struct intr_context*
__uproc_init_stack(uproc_entry_t entry)
{
  AUTO task = task_get_current();

  task->stack += sizeof(struct thread_context);
  struct intr_context* context = (struct intr_context*)task->stack;

  context->edi = 0;
  context->esi = 0;
  context->ebp = 0;
  context->esp = 0;
  context->esp_dummy = 0;
  context->ebx = 0;
  context->edx = 0;
  context->ecx = 0;
  context->eax = 0;

  context->gs = 0;
  context->fs = GDT_UDATA_SELECTOR;
  context->es = GDT_UDATA_SELECTOR;
  context->ds = GDT_UDATA_SELECTOR;

  context->eip = entry;
  context->cs = GDT_UCODE_SELECTOR;
  context->eflags = 0x202;

  vpage_link_addr(MEM_KERNEL_VSTART - MEM_PAGE_SIZE);
  context->esp = (void*)MEM_KERNEL_VSTART;

  context->ss = GDT_UDATA_SELECTOR;

  return context;
}

static void
__uproc_entry(void* arg)
{
  AUTO entry = (uproc_entry_t)arg;
  AUTO context = __uproc_init_stack(entry);

  __asm__ __volatile__("movl %0, %%esp;"
                       "jmp _asm_intr_common_handler_exit"
                       :
                       : "g"(context)
                       : "memory");
}

void
uproc_create(char* name, uproc_entry_t entry)
{
  bool res;

  // create a new task
  struct task* task = kmalloc(sizeof(struct task));
  if (!task) {
    KWARNING("failed to allocate user task %s", name);
    return;
  }

  // initialize the task
  task_init(task, name, TASK_DEFAULT_PRIORITY);

  // initialize the task kernel stack
  res = task_init_stack(task, __uproc_entry, entry);
  if (!res) {
    kfree(task);
    return;
  }

  // initialize the task page table
  res = task_init_page_table(task);
  if (!res) {
    buddy_free_page(page_get_by_phys(task->page_table), 0);
    kfree(task);
    return;
  }

  // push the task to the task list
  task_push(task);
  task_push_global(task);

  return;
}