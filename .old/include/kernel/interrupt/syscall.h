#pragma once

#include "lib/types.h"

typedef void* (*syscall_handler_t)(void*, void*, void*);

void
init_syscall(void);

/**
 * @brief Syscall entry point
 *
 * @param index Syscall index
 * @param arg1  First argument
 * @param arg2  Second argument
 * @param arg3  Third argument
 * @return void* Return value
 */
void*
syscall_common_handler(u32 index, void* arg1, void* arg2, void* arg3);