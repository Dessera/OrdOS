#pragma once

#include "lib/types.h"

/**
 * @brief Initialize the print subsystem
 */
void
init_print(void);

/**
 * @brief Print a string to the screen (thread safe)
 *
 * @param str The string to print
 */
void
kputs(const char* str);

/**
 * @brief Print a string to the screen (disable interrupts)
 *
 * @param str The string to print
 */
void
kputs_nint(const char* str);

/**
 * @brief Print a formatted string to the screen (thread safe)
 *
 * @param str The format string
 * @param ... The arguments
 *
 * @note The function donot check parameter count and type
 */
void
kprint(const char* str, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Print a formatted string to the screen (disable interrupts)
 *
 * @param str The format string
 * @param ... The arguments
 */
void
kprint_nint(const char* str, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Print a formatted string to the screen, with `\n` at the end (thread
 * safe)
 *
 * @param str The format string
 * @param ... The arguments
 *
 * @note The function donot check parameter count and type
 */
void
kprintln(const char* str, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Print a formatted string to the screen, with `\n` at the end (disable
 * interrupts)
 *
 * @param str The format string
 * @param ... The arguments
 */
void
kprintln_nint(const char* str, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Print a formatted string to the buffer
 *
 * @param buf The buffer to print to
 * @param str The format string
 * @param ... The arguments
 */
void
ksprint(char* buf, const char* str, ...) __attribute__((format(printf, 2, 3)));

/**
 * @brief Print a formatted string to the buffer, used to create other utils
 *
 * @param buf The buffer to print to
 * @param fmt The format string
 * @param args The arguments
 */
void
kvsprint(char* buf, const char* fmt, VA_LIST args);
