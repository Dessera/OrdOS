#pragma once

#include "kernel/types.h"

#ifdef DEBUG
#define KASSERT(expr)                                                          \
  if (!(expr)) {                                                               \
    kernel_panic_handler(__LINE__, __FILE__, __func__, #expr);                 \
  }
#define KASSERT_MSG(expr, msg)                                                 \
  if (!(expr)) {                                                               \
    kernel_panic_handler(__LINE__, __FILE__, __func__, msg);                   \
  }
#define KWARNON(expr)                                                          \
  if (!(expr)) {                                                               \
    kernel_warn_handler(__LINE__, __FILE__, __func__, #expr);                  \
  }
#define KWARNON_MSG(expr, msg)                                                 \
  if (!(expr)) {                                                               \
    kernel_warn_handler(__LINE__, __FILE__, __func__, msg);                    \
  }
#else
#define KASSERT(expr)
#define KASSERT_MSG(expr, msg)
#define KWARNON(expr)
#endif

#define KSTATIC_ASSERT(expr) _Static_assert(expr, #expr)
#define KSTATIC_ASSERT_MSG(expr, msg) _Static_assert(expr, msg)

#define KASSERT_NOT(expr) KASSERT(!(expr))
#define KASSERT_NOT_MSG(expr, msg) KASSERT_MSG(!(expr), msg)
#define KWARNON_NOT(expr) KWARNON(!(expr))
#define KWARNON_NOT_MSG(expr, msg) KWARNON_MSG(!(expr), msg)

void
kernel_panic_handler(u32 line,
                     const char* file,
                     const char* func,
                     const char* msg);

void
kernel_warn_handler(u32 line,
                    const char* file,
                    const char* func,
                    const char* msg);