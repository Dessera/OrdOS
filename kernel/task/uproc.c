#include "kernel/task/uproc.h"
#include "kernel/config/memory.h"
#include "kernel/config/task.h"
#include "kernel/memory/gdt.h"
#include "kernel/memory/page.h"
#include "kernel/task/context.h"
#include "kernel/task/task.h"
#include "kernel/utils/queue.h"

extern void (*_asm_intr_common_handler_exit)(void);

static void
__uproc_entry(void* arg)
{
  uproc_entry_t entry = (uproc_entry_t)arg;
  AUTO task = task_current();

  task->kstack += sizeof(struct thread_context);
  struct intr_context* context = (struct intr_context*)task->kstack;

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

  context->esp = link_page((void*)(MEM_KERNEL_VSTART - MEM_PAGE_SIZE), false) +
                 MEM_PAGE_SIZE;
  context->ss = GDT_UDATA_SELECTOR;

  __asm__ __volatile__("movl %0, %%esp;"
                       "jmp _asm_intr_common_handler_exit"
                       :
                       : "g"(context)
                       : "memory");
}

void
uproc_run(char* name, uproc_entry_t entry)
{
  struct task* task = alloc_page(TASK_PCB_PAGE_SIZE, true);
  task_init(task, name, TASK_DEFAULT_PRIORITY);
  task_init_stack(task, __uproc_entry, entry);
  task_init_vmmap(task);
  task_init_page_table(task);

  atomic_queue_push(&task_all_list, &task->global_node);
  atomic_queue_nint_push(&task_ready_list, &task->node);
}