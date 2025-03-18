
#include "kernel/utils/compiler.h"

extern uintptr_t _kernel_end[];

uintptr_t
compiler_get_kernel_end(void)
{
  return (uintptr_t)_kernel_end;
}