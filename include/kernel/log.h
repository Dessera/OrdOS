#pragma once

#include "kernel/types.h"
#include "kernel/utils/print.h"

// 0 : none, 1 : error, 2 : warning, 3 : info, 4 : debug 5 : trace
#ifndef LOGLEVEL
#define LOGLEVEL 3
#endif

// debug level to string helpers
const char* debug_level_str[] = { "NONE", "ERROR", "WARNING",
                                  "INFO", "DEBUG", "TRACE" };

#define KLOG(level, fmt, ...)                                                  \
  do {                                                                         \
    const char* level_str = level < 5 ? debug_level_str[level] : "UNKNOWN";    \
    kprint("[ %s ] %s:%u > ", level_str, __func__, __LINE__);                  \
    kprintln(fmt, ##__VA_ARGS__);                                              \
  } while (0)

#if LOGLEVEL < 5
#define KTRACE(fmt, ...)
#else
#define KTRACE(fmt, ...) KLOG(5, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < 4
#define KDEBUG(fmt, ...)
#else
#define KDEBUG(fmt, ...) KLOG(4, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < 3
#define KINFO(fmt, ...)
#else
#define KINFO(fmt, ...) KLOG(3, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < 2
#define KWARNING(fmt, ...)
#else
#define KWARNING(fmt, ...) KLOG(2, fmt, ##__VA_ARGS__)
#endif

#if LOGLEVEL < 1
#define KERROR(fmt, ...)
#else
#define KERROR(fmt, ...) KLOG(1, fmt, ##__VA_ARGS__)
#endif
