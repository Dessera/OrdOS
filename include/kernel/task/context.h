#pragma once

#include "kernel/task/task.h"
#include "kernel/types.h"
struct intr_context_t
{
  u32 index;
  u32 edi;
  u32 esi;
  u32 ebp;
  u32 esp_dummy;
  u32 ebx;
  u32 edx;
  u32 ecx;
  u32 eax;
  u32 gs;
  u32 fs;
  u32 es;
  u32 ds;

  // another index (duplicate because of vec generation)
  u32 index_dup;
  u32 error_code;
  void (*eip)(void);
  u32 cs;
  u32 eflags;
  void* esp;
  u32 ss;
};

struct thread_context_t
{
  u32 ebp;
  u32 ebx;
  u32 edi;
  u32 esi;

  void (*eip)(task_function_t func, void* arg);

  void* ret_addr;
  task_function_t func;
  void* arg;
};