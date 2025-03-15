#pragma once

#include "lib/asm.h"
#include "lib/types.h"

#define SYSCALL_INDEX(syscall) ((u32)(syscall))

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

enum syscall_enumerate
{
  SYSCALL_GETPID,
  SYSCALL_WRITE
};

size_t
getpid(void);

// temporary write syscall
ssize_t
write(size_t fd, char* buf, size_t len);