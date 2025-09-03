/**
 * @file task.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel task management.
 * @version 0.1.0
 * @date 2025-08-29
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/config.h"
#include "ordos/core/mem/vpage.h"
#include "ordos/lib/types.h" // IWYU pragma: keep
#include "ordos/lib/util/list_head.h"

/**
 * @brief Task priority type.
 *
 */
typedef u8 task_prio_t;

/**
 * @brief Task entry type.
 *
 */
typedef void (*task_entry_t)(void*);

/**
 * @brief Task status type.
 *
 */
enum task_status
{
  TSK_RUNNING,
  TSK_READY,
  TSK_BLOCKED,
  TSK_DEAD,
};

/**
 * @brief Task struct.
 *
 */
struct task
{
  void* stack;
  size_t pid;
  char name[ORDOS_TASK_NAME_LENGTH];
  enum task_status status;
  task_prio_t prio;

  size_t ticks;
  size_t vticks;

  struct list_head node;
  struct list_head global_node;

  pde_t* pd;
};

/**
 * @brief Init blank task.
 *
 * @param task Task struct.
 * @param name Task name.
 * @param priority Task priority.
 */
void
task_init(struct task* task, const char* name, task_prio_t priority);

/**
 * @brief Init task kernel stack.
 *
 * @param task Task struct.
 * @param function Task entry.
 * @param arg Task args.
 * @return int Init status.
 */
int
task_init_stack(struct task* task, task_entry_t function, void* arg);

/**
 * @brief Push a task to the global queue.
 *
 * @param task Task to push.
 */
void
task_push_global(struct task* task);

/**
 * @brief Push a task to the ready queue.
 *
 * @param task Task to push.
 */
void
task_push(struct task* task);

/**
 * @brief Set the current task.
 *
 * @param task Task to set as current.
 */
void
task_set_current(struct task* task);

/**
 * @brief Get the current task.
 *
 * @return struct task* Current task.
 */
struct task*
task_get_current(void);

/**
 * @brief Yield the current task.
 */
void
task_yield(void);

/**
 * @brief Block current task.
 */
void
task_park(void);

/**
 * @brief Unblock a task.
 *
 * @param task Task to unblock.
 */
void
task_unpark(struct task* task);

void
_asm_task_switch_to(struct task* curr, struct task* next);
