#pragma once

#include "kernel/types.h"

void
init_syscall(void);

size_t
syscall_getpid(void);

/**
 * @brief Syscall entry point
 *
 * @param index Syscall index
 * @param arg1  First argument
 * @param arg2  Second argument
 * @return void* Return value
 */
void*
syscall_common_handler(u32 index, void* arg1, void* arg2);