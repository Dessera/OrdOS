#pragma once

#include "kernel/interrupt/interrupt.h" // for intr_set_status
#include "kernel/types.h"
#include "kernel/utils/print.h" // for kprintln

#define LOGLEVEL_NONE 0
#define LOGLEVEL_ERROR 1
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_INFO 3
#define LOGLEVEL_DEBUG 4
#define LOGLEVEL_TRACE 5

#define LOGLEVEL_STR_NONE "NONE"
#define LOGLEVEL_STR_ERROR "ERROR"
#define LOGLEVEL_STR_WARNING "WARNING"
#define LOGLEVEL_STR_INFO "INFO"
#define LOGLEVEL_STR_DEBUG "DEBUG"
#define LOGLEVEL_STR_TRACE "TRACE"
#define LOGLEVEL_STR_PANIC "PANIC"

// used only for assert
#define LOGLEVEL_PANIC 6

#ifndef LOGLEVEL
#define LOGLEVEL LOGLEVEL_INFO
#endif

#define KLOG_WITH(level_str, func, line, fmt, ...)                             \
  kprintln("[ %s ] %s:%u > " fmt, level_str, func, line, ##__VA_ARGS__)

#define KLOG(level_str, fmt, ...)                                              \
  KLOG_WITH(level_str, __func__, __LINE__, fmt, ##__VA_ARGS__)

#if LOGLEVEL < LOGLEVEL_TRACE
#define KTRACE(fmt, ...)
#else
#define KTRACE(fmt, ...) KLOG(LOGLEVEL_STR_TRACE, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_DEBUG
#define KDEBUG(fmt, ...)
#else
#define KDEBUG(fmt, ...) KLOG(LOGLEVEL_STR_DEBUG, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_INFO
#define KINFO(fmt, ...)
#else
#define KINFO(fmt, ...) KLOG(LOGLEVEL_STR_INFO, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_WARNING
#define KWARNING(fmt, ...)
#else
#define KWARNING(fmt, ...) KLOG(LOGLEVEL_STR_WARNING, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_ERROR
#define KERROR(fmt, ...)
#else
#define KERROR(fmt, ...) KLOG(LOGLEVEL_STR_ERROR fmt, ##__VA_ARGS__)
#endif

#define KPANIC(fmt, ...)                                                       \
  do {                                                                         \
    intr_set_status(false);                                                    \
    KLOG(LOGLEVEL_STR_PANIC, fmt, ##__VA_ARGS__);                              \
    while (true)                                                               \
      ;                                                                        \
  } while (0)
