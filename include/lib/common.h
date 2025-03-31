#pragma once

#define EXPAND(...) __VA_ARGS__
#define EXPAND1(...) EXPAND(EXPAND(EXPAND(EXPAND(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND5(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))

#define PARENS ()

#define RECURSIVE_APPLY(macro, sep, ...)                                       \
  __VA_OPT__(EXPAND5(RECURSIVE_APPLY_HELPER(macro, sep, __VA_ARGS__)))
#define RECURSIVE_APPLY_HELPER(macro, sep, x, ...)                             \
  macro(x) __VA_OPT__(sep RECURSIVE_APPLY_NEXT PARENS(macro, sep, __VA_ARGS__))
#define RECURSIVE_APPLY_NEXT() RECURSIVE_APPLY_HELPER

#define RECURSIVE_APPLY_WITH_COMMA(macro, ...)                                 \
  __VA_OPT__(EXPAND5(RECURSIVE_APPLY_WITH_COMMA_HELPER(macro, __VA_ARGS__)))
#define RECURSIVE_APPLY_WITH_COMMA_HELPER(macro, x, ...)                       \
  macro(x)                                                                     \
    __VA_OPT__(, RECURSIVE_APPLY_WITH_COMMA_NEXT PARENS(macro, __VA_ARGS__))
#define RECURSIVE_APPLY_WITH_COMMA_NEXT() RECURSIVE_APPLY_WITH_COMMA_HELPER

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

#define __CAT(x, y) x##y
#define CAT(x, y) __CAT(x, y)

#define FORWARD(x) x

#define MEMKB(x) ((x) * 1024)
#define MEMMB(x) (MEMKB(x) * 1024)
#define MEMGB(x) (MEMMB(x) * 1024)

#define TMSEC(x) ((x) * 1000)
#define TMIN(x) (TMSEC(x) * 60)
#define THOUR(x) (TMIN(x) * 60)

#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_DOWN(x, a) ((x) & ~((a) - 1))

#define DIV_UP(x, y) (((x) + (y) - 1) / (y))
#define DIV_DOWN(x, y) ((x) / (y))

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
// #define MIN(x, y)
//   ({
//     typeof(x) _x = (x);
//     typeof(y) _y = (y);
//     (typeof(_x + _y))(_x < _y ? _x : _y);
//   })
// #define MAX(x, y)
//   ({
//     typeof(x) _x = (x);
//     typeof(y) _y = (y);
//     (typeof(_x + _y))(_x > _y ? _x : _y);
//   })
