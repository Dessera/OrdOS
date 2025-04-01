#pragma once

#include "kernel/config/task.h"
#include "kernel/device/clk.h"
#include "lib/common.h"
#include "lib/list_head.h"
#include "lib/types.h"

typedef void (*task_entry_t)(void*);

/**
 * @brief Task status enumeration
 */
enum task_status
{
  TASK_STATUS_RUNNING,
  TASK_STATUS_READY,
  TASK_STATUS_BLOCKED,
  TASK_STATUS_DEAD,
};

/**
 * @brief Task structure
 */
struct task
{
  void* stack;
  size_t pid;
  enum task_status status;
  char name[TASK_NAME_SIZE];

  u8 priority;
  size_t ticks;
  size_t elapsed_ticks;

  struct list_head node;
  struct list_head global_node;

  uintptr_t page_table;
  i32 fds[TASK_FD_SIZE];
};

/**
 * @brief Initialize the task management subsystem
 */
void
init_task(void);

/**
 * @brief Initialize a task
 *
 * @param task Task to initialize
 * @param name Task name
 * @param priority Task priority
 */
void
task_init(struct task* task, const char* name, u8 priority);

/**
 * @brief Initialize kernel stack for a task
 *
 * @param task Task to initialize
 * @param function Entry function
 * @param arg Argument to pass to entry function
 * @return true if success
 * @return false if failure
 */
bool
task_init_stack(struct task* task, task_entry_t function, void* arg);

/**
 * @brief Initialize an unique page table for a task
 *
 * @param task Task to initialize
 * @return true if success
 * @return false if failure
 */
bool
task_init_page_table(struct task* task);

/**
 * @brief Push a task to the global queue
 *
 * @param task Task to push
 */
void
task_push_global(struct task* task);

/**
 * @brief Push a task to the ready queue
 *
 * @param task Task to push
 */
void
task_push(struct task* task);

/**
 * @brief Set the current task
 *
 * @param task Task to set as current
 */
void
task_set_current(struct task* task);

/**
 * @brief Get the current task
 *
 * @return struct task* Current task
 */
struct task*
task_get_current(void);

/**
 * @brief Yield the current task
 */
void
task_yield(void);

/**
 * @brief Block current task
 */
void
task_park(void);

/**
 * @brief Unblock a task
 *
 * @param task Task to unblock
 */
void
task_unpark(struct task* task);

/**
 * @brief Sleep current task for a given number of ticks
 *
 * @param ticks Number of ticks to sleep
 */
void
task_sleep_ticks(size_t ticks);

/**
 * @brief Sleep current task for a given number of milliseconds
 *
 * @param ms Number of milliseconds to sleep
 */
static FORCE_INLINE void
task_sleep_millses(size_t ms)
{
  task_sleep_ticks(ms * SYS_CLK_INTR_FREQ / TMSEC(1));
}

/**
 * @brief Sleep current task for a given number of seconds
 *
 * @param seconds Number of seconds to sleep
 */
static FORCE_INLINE void
task_sleep_seconds(size_t seconds)
{
  task_sleep_ticks(seconds * SYS_CLK_INTR_FREQ);
}