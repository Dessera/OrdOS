#include "lib/syscall.h"
#include "lib/types.h"

size_t
getpid(void)
{
  size_t ret;
  SYSCALL_A0(SYSCALL_GETPID, ret);
  return ret;
}

size_t
write(char* buf)
{
  size_t ret;
  SYSCALL_A1(SYSCALL_WRITE, ret, buf);
  return ret;
}