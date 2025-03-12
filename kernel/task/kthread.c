#include "kernel/task/kthread.h"
#include "kernel/config/task.h"
#include "kernel/memory/page.h"
#include "kernel/task/task.h"
#include "kernel/types.h"
#include "kernel/utils/queue.h"

static struct task* main_thread;

static void
__init_main_thread(void)
{
  main_thread = task_current();

  task_init(main_thread, TASK_KTHREAD_MAIN_NAME, TASK_KTHREAD_MAIN_PRIORITY);
  main_thread->status = TASK_STATUS_RUNNING;

  atomic_queue_push(&task_all_list, &main_thread->global_node);
  task_running = &main_thread->node;
}

void
init_kthread(void)
{
  __init_main_thread();
}

struct task*
kthread_run(char* name, u8 priority, task_entry_t function, void* arg)
{
  struct task* task = alloc_page(TASK_PCB_PAGE_SIZE, true);
  task_init(task, name, priority);
  task_init_stack(task, function, arg);

  atomic_queue_nint_push(&task_ready_list, &task->node);
  atomic_queue_push(&task_all_list, &task->global_node);

  return task;
}
