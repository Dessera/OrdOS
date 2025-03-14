#include "kernel/interrupt/syscall.h"
#include "kernel/utils/print.h"

void*
syscall_common_handler(u32 index, void* arg1, void* arg2)
{
  kputs(".");
  (void)index;
  (void)arg1;
  (void)arg2;
  return (void*)42;
}