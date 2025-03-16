#pragma once

#include "kernel/task/task.h"
#include "lib/types.h"

struct intr_context
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

struct thread_context
{
  u32 ebp;
  u32 ebx;
  u32 edi;
  u32 esi;

  void (*eip)(task_entry_t func, void* arg);

  void* ret_addr;
  task_entry_t func;
  void* arg;
};

struct tss_context
{
  u32 backlink;
  u32* esp0;
  u32 ss0;
  u32* esp1;
  u32 ss1;
  u32* esp2;
  u32 ss2;
  u32 cr3;
  u32 (*eip)(void);
  u32 eflags;
  u32 eax;
  u32 ecx;
  u32 edx;
  u32 ebx;
  u32 esp;
  u32 ebp;
  u32 esi;
  u32 edi;
  u32 es;
  u32 cs;
  u32 ss;
  u32 ds;
  u32 fs;
  u32 gs;
  u32 ldtr;
  u32 trace;
  u32 io_base;
};
