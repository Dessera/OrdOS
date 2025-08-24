#pragma once

#include "ordos/kernel/logging.h"

#ifdef DEBUG
#define kassert(expr, msg, ...)                                                \
  do {                                                                         \
    if (!(expr)) {                                                             \
      kpanic(msg, ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)
#else
#define kassert(expr, msg, ...)
#endif

#ifdef DEBUG
#define kassert_warn(expr, msg, ...)                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      kwarn(msg, ##__VA_ARGS__);                                               \
    }                                                                          \
  } while (0)
#else
#define kassert_warn(expr, msg, ...)
#endif

#define kstatic_assert(expr, msg) _Static_assert(expr, msg)

#define kassert_expr(expr) KASSERT(expr, "assertion failed: %s", #expr)

#define kassert_not(expr, msg, ...) KASSERT(!(expr), msg, ##__VA_ARGS__)
