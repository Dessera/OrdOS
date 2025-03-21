
#include "kernel/utils/compiler.h"
#include "lib/types.h"

extern uintptr_t _kernel_end[];

FORCE_INLINE uintptr_t
compiler_get_kernel_end(void)
{
  return (uintptr_t)_kernel_end;
}