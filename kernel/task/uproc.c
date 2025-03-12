#include "kernel/task/uproc.h"
#include "kernel/config/task.h"
#include "kernel/memory/page.h"
#include "kernel/task/task.h"

void
__uproc_entry(void* arg)
{
}

void
uproc_run(char* name, void (*entry)(void))
{
  struct task* task = alloc_page(TASK_PCB_PAGE_SIZE, true);
  task_init(task, name, TASK_DEFAULT_PRIORITY);
  task_init_stack(task, __uproc_entry, entry);
}