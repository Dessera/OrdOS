#pragma once

#define TASK_NAME_SIZE 16
#define TASK_STACK_PAGE_SIZE 1
#define TASK_MAGIC 0x544342

#define TASK_KTHREAD_MAIN_NAME "kthread_main"
#define TASK_DEFAULT_PRIORITY 31
#define TASK_IDLE_NAME "kthread_idle"
#define TASK_IDLE_PRIORITY 10

#define TASK_PID_BITMAP_SIZE 128
#define TASK_FD_SIZE 64