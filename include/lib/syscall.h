#pragma once

#include "lib/types.h"

enum syscall_enumerate
{
  SYSCALL_GETPID,
  SYSCALL_WRITE
};

size_t
getpid(void);

// temporary write syscall
size_t
write(size_t fd, const char* buf, size_t len);