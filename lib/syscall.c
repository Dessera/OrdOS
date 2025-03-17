#include "lib/syscall.h"
#include "lib/types.h"

#define SYSCALL_A0(syscall, ret)                                               \
  ASM("int $0x80" : "=a"(ret) : "a"(syscall) : "memory")

#define SYSCALL_A1(syscall, ret, arg1)                                         \
  ASM("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg1) : "memory")

#define SYSCALL_A2(syscall, ret, arg1, arg2)                                   \
  ASM("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg1), "c"(arg2) : "memory")

#define SYSCALL_A3(syscall, ret, arg1, arg2, arg3)                             \
  ASM("int $0x80" : "=a"(ret) : "a"(syscall),                                  \
      "b"(arg1),                                                               \
      "c"(arg2),                                                               \
      "d"(arg3) : "memory")

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