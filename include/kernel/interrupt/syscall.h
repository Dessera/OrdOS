#pragma once

typedef void* (*syscall_handler_t)(void*, void*, void*);

/**
 * @brief Register a syscall handler
 */
void
init_syscall(void);
