#pragma once

#include "lib/types.h"

ENUM_DECLARE(intr_syscall, SYSCALL_GETPID, SYSCALL_WRITE)

size_t
getpid(void);

// temporary write syscall
size_t
write(size_t fd, const char* buf, size_t len);