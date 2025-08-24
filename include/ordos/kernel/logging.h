/**
 * @file logging.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel loggers.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/boot/prelude.h"
#include "ordos/lib/asm.h" // IWYU pragma: keep
#include "ordos/lib/common.h"

#define LOGLEVEL_NONE 0
#define LOGLEVEL_ERROR 1
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_INFO 3
#define LOGLEVEL_DEBUG 4
#define LOGLEVEL_TRACE 5

#define LOGLEVEL_PANIC 6

#define LOGLEVEL_STR_ERROR "ERROR"
#define LOGLEVEL_STR_WARNING "WARNING"
#define LOGLEVEL_STR_INFO "INFO"
#define LOGLEVEL_STR_DEBUG "DEBUG"
#define LOGLEVEL_STR_TRACE "TRACE"
#define LOGLEVEL_STR_PANIC "PANIC"

#ifndef LOGLEVEL
#define LOGLEVEL LOGLEVEL_INFO
#endif

#define __klog_with(logger, level_str, fmt, ...)                               \
  logger("[ " level_str " ] " fmt, ##__VA_ARGS__)

#define __klog(level_str, fmt, ...)                                            \
  __klog_with(kprintln, level_str, fmt, ##__VA_ARGS__)

#define __klog_unsafe(level_str, fmt, ...)                                     \
  __klog_with(kprintln_unsafe, level_str, fmt, ##__VA_ARGS__)

#if LOGLEVEL < LOGLEVEL_TRACE
#define ktrace(fmt, ...)
#define ktrace_unsafe(fmt, ...)
#else
#define ktrace(fmt, ...) __klog(LOGLEVEL_STR_TRACE, fmt, ##__VA_ARGS__)
#define ktrace_unsafe(fmt, ...)                                                \
  __klog_unsafe(LOGLEVEL_STR_TRACE, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_DEBUG
#define kdebug(fmt, ...)
#define kdebug_unsafe(fmt, ...)
#else
#define kdebug(fmt, ...) __klog(LOGLEVEL_STR_DEBUG, fmt, ##__VA_ARGS__)
#define kdebug_unsafe(fmt, ...)                                                \
  __klog_unsafe(LOGLEVEL_STR_DEBUG, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_INFO
#define kinfo(fmt, ...)
#define kinfo_unsafe(fmt, ...)
#else
#define kinfo(fmt, ...) __klog(LOGLEVEL_STR_INFO, fmt, ##__VA_ARGS__)
#define kinfo_unsafe(fmt, ...)                                                 \
  __klog_unsafe(LOGLEVEL_STR_INFO, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_WARNING
#define kwarn(fmt, ...)
#define kwarn_unsafe(fmt, ...)
#else
#define kwarn(fmt, ...) __klog(LOGLEVEL_STR_WARNING, fmt, ##__VA_ARGS__)
#define kwarn_unsafe(fmt, ...)                                                 \
  __klog_unsafe(LOGLEVEL_STR_WARNING, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_ERROR
#define kerror(fmt, ...)
#define kerror_unsafe(fmt, ...)
#else
#define kerror(fmt, ...) __klog(LOGLEVEL_STR_ERROR, fmt, ##__VA_ARGS__)
#define kerror_unsafe(fmt, ...)                                                \
  __klog_unsafe(LOGLEVEL_STR_ERROR, fmt, ##__VA_ARGS__)
#endif

#define kpanic(fmt, ...)                                                       \
  do {                                                                         \
    __klog_unsafe(LOGLEVEL_STR_PANIC, fmt, ##__VA_ARGS__);                     \
    while (1) {                                                                \
      hlt();                                                                   \
    }                                                                          \
  } while (0)

/**
 * @brief Panic in prelude.
 *
 */
#define kpanic_prelude(str)                                                    \
  do {                                                                         \
    kprelude_puts(str);                                                        \
    while (1) {                                                                \
      hlt();                                                                   \
    }                                                                          \
  } while (0)

/**
 * @brief Initialize the print subsystem
 */
void
init_print(void);

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
