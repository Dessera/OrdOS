#pragma once

#include "kernel/config/task.h"
#include "kernel/memory/page.h"
#include "kernel/types.h"
#include "kernel/utils/list_head.h"

/**
 * @brief Task status enumeration
 */
enum task_status
{
  TASK_STATUS_RUNNING,
  TASK_STATUS_READY,
  TASK_STATUS_BLOCKED,
  TASK_STATUS_WAITING,
  TASK_STATUS_SUSPENDED,
  TASK_STATUS_DEAD,
};

/**
 * @brief Function type for task entry point
 */
typedef void (*task_entry_t)(void*);

/**
 * @brief Task structure, representing a task in the system (either user process
 * or kernel thread)
 */
struct task
{
  u32* kstack;
  enum task_status status;
  char name[TASK_NAME_SIZE];

  u8 priority;
  size_t ticks;
  size_t elapsed_ticks;

  struct list_head node;
  struct list_head global_node;

  u32* page_table;
  struct vmemmap user_vaddr;

  u32 tmagic;
};

extern struct atomic_queue_nint task_ready_list;
extern struct atomic_queue task_all_list;
extern struct list_head* task_running;

void
init_task(void);

/**
 * @brief Get the current running task
 *
 * @return struct task* The current running task
 */
struct task*
task_current(void);

/**
 * @brief Create a new task
 *
 * @param task Task to create
 * @param name Task name
 * @param priority Task priority
 */
void
task_init(struct task* task, char* name, u8 priority); // 初始化任务

/**
 * @brief Init task's stack for kernel space
 *
 * @param task Task to init
 * @param function Task entry point
 * @param arg Argument for task entry point
 */
void
task_init_stack(struct task* task, task_entry_t function, void* arg);

/**
 * @brief Init task's virtual memory map for user space
 *
 * @param task Task to init
 */
void
task_init_vmmap(struct task* task);

/**
 * @brief Switch to another task
 */
void
task_yield(void);

/**
 * @brief Park a task (i.e. make it blocked)
 */
void
task_park(void);

/**
 * @brief Unpark a task (i.e. make it runnable again)
 *
 * @param task Task to unpark
 */
void
task_unpark(struct task* task);