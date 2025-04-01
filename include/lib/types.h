#pragma once

#ifndef __ASSEMBLER__

#include "lib/common.h"

#define OFFSET_OF(type, member) __builtin_offsetof(type, member)

#define CONTAINER_OF(ptr, type, member)                                        \
  ({                                                                           \
    typeof(((type*)0)->member)* mptr = (ptr);                                  \
    (type*)((char*)mptr - OFFSET_OF(type, member));                            \
  })

#define VA_LIST __builtin_va_list
#define VA_START __builtin_va_start
#define VA_END __builtin_va_end
#define VA_ARG __builtin_va_arg

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef u32 size_t;
typedef i32 ssize_t;

typedef u32 uintptr_t;
typedef i32 intptr_t;

#define MAX_U32 ((u32) - 1)
#define MIN_U32 ((u32)0)
#define MAX_SIZE_T ((size_t)-1)
#define MIN_SIZE_T ((size_t)0)

#define MAX_SSIZE_T ((ssize_t)0x7FFFFFFF)
#define MIN_SSIZE_T ((ssize_t)0x80000000)

#define NPOS ((ssize_t) - 1)

#define AUTO __auto_type

#define FORCE_INLINE __attribute__((always_inline)) inline

#define CLEAN_UP(cleaner) __attribute__((cleanup(cleaner)))

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define DECLARE_WITH_PROTOTYPE(ret, name, ...)                                 \
  ret name(__VA_ARGS__);                                                       \
  ret name(__VA_ARGS__)

#define ENUM_KEY(x) x
#define ENUM_KEY_STR(x) STRINGIFY(x)
#define ENUM_INC(x) +1

#define ENUM_DECLARE(enum_name, ...)                                           \
  enum enum_name                                                               \
  {                                                                            \
    RECURSIVE_APPLY_WITH_COMMA(ENUM_KEY, __VA_ARGS__)                          \
  };                                                                           \
                                                                               \
  static FORCE_INLINE const char* enum_name##_to_str(enum enum_name x)         \
  {                                                                            \
    static const char* enum_name##_str[] = { RECURSIVE_APPLY_WITH_COMMA(       \
      ENUM_KEY_STR, __VA_ARGS__) };                                            \
    return enum_name##_str[x];                                                 \
  }                                                                            \
                                                                               \
  static FORCE_INLINE size_t enum_name##_get_size(void)                        \
  {                                                                            \
    return RECURSIVE_APPLY(ENUM_INC, , __VA_ARGS__);                           \
  }

void
itoa(char* buffer, i32 value, u8 base);

void
utoa(char* buffer, u32 value, u8 base);

#endif // __ASSEMBLER__
