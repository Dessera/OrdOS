#pragma once

#include "ordos/lib/common.h"
#include "ordos/lib/section.h"

/**
 * @brief Print a string to the screen (thread safe).
 *
 * @param str The string to print.
 */
void
kputs(const char* str);

/**
 * @brief Print a string to the screen (thread unsafe).
 *
 * @param str The string to print.
 */
void
kputs_unsafe(const char* str);

/**
 * @brief Print a formatted string to the screen (thread safe).
 *
 * @param str The format string.
 * @param ... The arguments.
 *
 * @note The function donot check parameter count and type.
 */
__format(printf, 1, 2) void kprint(const char* str, ...);

/**
 * @brief Print a formatted string to the screen (thread unsafe).
 *
 * @param str The format string.
 * @param ... The arguments.
 */
__format(printf, 1, 2) void kprint_unsafe(const char* str, ...);

/**
 * @brief Print a formatted string to the screen, with `\n` at the end (thread
 * safe).
 *
 * @param str The format string.
 * @param ... The arguments.
 *
 * @note The function donot check parameter count and type.
 */
__format(printf, 1, 2) void kprintln(const char* str, ...);

/**
 * @brief Print a formatted string to the screen, with `\n` at the end (thread
 * unsafe).
 *
 * @param str The format string.
 * @param ... The arguments.
 */
__format(printf, 1, 2) void kprintln_unsafe(const char* str, ...);

/**
 * @brief Print a formatted string to the buffer.
 *
 * @param buf The buffer to print to.
 * @param str The format string.
 * @param ... The arguments.
 */
__format(printf, 2, 3) void ksprint(char* buf, const char* str, ...);

/**
 * @brief Print a formatted string to the buffer, used to create other utils.
 *
 * @param buf The buffer to print to.
 * @param fmt The format string.
 * @param args The arguments.
 */
void
kvsprint(char* buf, const char* fmt, va_list args);

/**
 * @brief Print a string to the screen (for prelude).
 *
 * @param str The string to print (should be a vm addr).
 */
__prelude void
kprelude_puts(const char* str);
