/**
 * @file syscall.h
 * @author Dessera (dessera@qq.com)
 * @brief OrdOS syscalls.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

/**
 * @brief Common syscall handler.
 */
typedef void* (*syscall_handler_t)(void*, void*, void*);

/**
 * @brief Register a syscall handler.
 */
void
init_syscall(void);
