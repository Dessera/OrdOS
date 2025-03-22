#include "kernel/task/uproc.h"
#include "kernel/config/memory.h"
#include "kernel/config/task.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/gdt.h"
#include "kernel/memory/memory.h"
#include "kernel/memory/vpage.h"
#include "kernel/task/context.h"
#include "kernel/task/task.h"

extern void (*_asm_intr_common_handler_exit)(void);

void
__uproc_entry(void* arg);

void
__uproc_entry(void* arg)
{
  uproc_entry_t entry = (uproc_entry_t)arg;
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

  // AUTO page_table = (u32*)((uintptr_t)task->page_table + MEM_KERNEL_VSTART);
  // AUTO pte_page = buddy_alloc_page(MEM_ZONE_NORMAL, 0);
  // AUTO stack_page = buddy_alloc_page(MEM_ZONE_NORMAL, 0);

  // page_table[0x2ff] = PAGE_PDE_DESC(page_get_phys(pte_page), 1, 1, 1);

  // u32* ptes = (u32*)(page_get_virt(pte_page));
  // ptes[0x3ff] = PAGE_PTE_DESC(page_get_phys(stack_page), 1, 1, 1);
  vpage_link_addr(MEM_KERNEL_VSTART - MEM_PAGE_SIZE);
  context->esp = (void*)MEM_KERNEL_VSTART;

  context->ss = GDT_UDATA_SELECTOR;

  __asm__ __volatile__("movl %0, %%esp;"
                       "jmp _asm_intr_common_handler_exit"
                       :
                       : "g"(context)
                       : "memory");
}

void
uproc_create(char* name, uproc_entry_t entry)
{
  struct task* task = kmalloc(sizeof(struct task));
  task_init(task, name, TASK_DEFAULT_PRIORITY);
  task_init_stack(task, __uproc_entry, entry);
  task_init_page_table(task);

  task_push(task);
  task_push_global(task);
}