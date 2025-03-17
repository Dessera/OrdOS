#pragma once

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

#define __CAT(x, y) x##y
#define CAT(x, y) __CAT(x, y)

#define FORWARD(x) x

#define MEMKB(x) ((x) * 1024)
#define MEMMB(x) (MEMKB(x) * 1024)
#define MEMGB(x) (MEMMB(x) * 1024)

#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_DOWN(x, a) ((x) & ~((a) - 1))

#define DIV_UP(x, y) (((x) + (y) - 1) / (y))
#define DIV_DOWN(x, y) ((x) / (y))

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))