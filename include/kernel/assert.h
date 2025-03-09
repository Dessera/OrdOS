#pragma once

#include "kernel/log.h"

#ifdef DEBUG
#define KASSERT(expr, msg, ...)                                                \
  if (!(expr)) {                                                               \
    KPANIC(msg, ##__VA_ARGS__);                                                \
  }
#else
#define KASSERT(expr, msg, ...)
#endif

#define KSTATIC_ASSERT(expr, msg) _Static_assert(expr, msg)

#define KASSERT_EXPR(expr) KASSERT(expr, "assertion failed: %s", #expr)

#define KASSERT_NOT(expr, msg, ...) KASSERT(!(expr), msg, ##__VA_ARGS__)
