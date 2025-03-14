#include "user/syscall.h"
#include "lib/types.h"

size_t
getpid(void)
{
  size_t ret;
  SYSCALL_A0(SYSCALL_GETPID, ret);
  return ret;
}