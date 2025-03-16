#include "lib/syscall.h"
#include "lib/types.h"

size_t
getpid(void)
{
  size_t ret;
  SYSCALL_A0(SYSCALL_GETPID, ret);
  return ret;
}

ssize_t
write(size_t fd, char* buf, size_t len)
{
  ssize_t ret;
  SYSCALL_A3(SYSCALL_WRITE, ret, fd, buf, len);
  return ret;
}