#pragma once

#include "kernel/interrupt/interrupt.h" // for intr_set_status
#include "kernel/task/sync.h"
#include "kernel/types.h"
#include "kernel/utils/asm.h"   // for hlt
#include "kernel/utils/print.h" // for kprint, kprintln

#define LOGLEVEL_NONE 0
#define LOGLEVEL_ERROR 1
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_INFO 3
#define LOGLEVEL_DEBUG 4
#define LOGLEVEL_TRACE 5

// used only for assert
#define LOGLEVEL_PANIC 6

#ifndef LOGLEVEL
#define LOGLEVEL LOGLEVEL_INFO
#endif

// debug level to string helpers
extern const char* debug_level_str[7];
extern struct lock_t klog_lock;

#define KLOG_WITH(level, func, line, fmt, ...)                                 \
  do {                                                                         \
    const char* level_str = level <= 6 ? debug_level_str[level] : "UNKNOWN";   \
    lock(&klog_lock);                                                          \
    kprint("[ %s ] %s:%u > ", level_str, func, line);                          \
    kprintln(fmt, ##__VA_ARGS__);                                              \
    unlock(&klog_lock);                                                        \
  } while (0)

#define KLOG(level, fmt, ...)                                                  \
  KLOG_WITH(level, __func__, __LINE__, fmt, ##__VA_ARGS__)

#if LOGLEVEL < LOGLEVEL_TRACE
#define KTRACE(fmt, ...)
#else
#define KTRACE(fmt, ...) KLOG(LOGLEVEL_TRACE, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_DEBUG
#define KDEBUG(fmt, ...)
#else
#define KDEBUG(fmt, ...) KLOG(LOGLEVEL_DEBUG, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_INFO
#define KINFO(fmt, ...)
#else
#define KINFO(fmt, ...) KLOG(LOGLEVEL_INFO, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_WARNING
#define KWARNING(fmt, ...)
#else
#define KWARNING(fmt, ...) KLOG(LOGLEVEL_WARNING, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < LOGLEVEL_ERROR
#define KERROR(fmt, ...)
#else
#define KERROR(fmt, ...) KLOG(LOGLEVEL_ERROR fmt, ##__VA_ARGS__)
#endif

#define KPANIC(fmt, ...)                                                       \
  do {                                                                         \
    intr_set_status(false);                                                    \
    KLOG(LOGLEVEL_PANIC, fmt, ##__VA_ARGS__);                                  \
    while (true) {                                                             \
      hlt();                                                                   \
    }                                                                          \
  } while (0)

void
init_log(void);
