#pragma once

#include "lib/asm.h"
#include "lib/types.h"

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