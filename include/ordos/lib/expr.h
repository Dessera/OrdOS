/**
 * @file expr.h
 * @author Dessera (dessera@qq.com)
 * @brief Expression utils.
 * @version 0.1.0
 * @date 2025-08-27
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#define PARENS ()

#define __expand(...) __VA_ARGS__
#define __expand1(...) __expand(__expand(__expand(__expand(__VA_ARGS__))))
#define __expand2(...) __expand1(__expand1(__expand1(__expand1(__VA_ARGS__))))
#define __expand3(...) __expand2(__expand2(__expand2(__expand2(__VA_ARGS__))))
#define __expand4(...) __expand3(__expand3(__expand3(__expand3(__VA_ARGS__))))
#define __expand5(...) __expand4(__expand4(__expand4(__expand4(__VA_ARGS__))))

#define __recursive_apply(macro, sep, ...)                                     \
  __VA_OPT__(__expand5(__recursive_apply_helper(macro, sep, __VA_ARGS__)))

#define __recursive_apply_helper(macro, sep, x, ...)                           \
  macro(x)                                                                     \
    __VA_OPT__(sep __recursive_apply_next PARENS(macro, sep, __VA_ARGS__))

#define __recursive_apply_next() __recursive_apply_helper

#define __recursive_apply_with_comma(macro, ...)                               \
  __VA_OPT__(__expand5(__recursive_apply_with_comma_helper(macro, __VA_ARGS__)))

#define __recursive_apply_with_comma_helper(macro, x, ...)                     \
  macro(x)                                                                     \
    __VA_OPT__(, __recursive_apply_with_comma_next PARENS(macro, __VA_ARGS__))

#define __recursive_apply_with_comma_next() __recursive_apply_with_comma_helper
