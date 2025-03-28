#pragma once

#include "lib/types.h" // IWYU pragma: keep

/**
 * @brief Common syscall handler
 */
typedef void* (*syscall_handler_t)(void*, void*, void*);

/**
 * @brief Register a syscall handler
 */
void
init_syscall(void);

size_t
sys_getpid(void);

size_t
sys_write(size_t fd, const void* buf, size_t count);