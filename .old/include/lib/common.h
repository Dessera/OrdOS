#pragma once

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

#define __CAT(x, y) x##y
#define CAT(x, y) __CAT(x, y)

#define FORWARD(x) x

#define MEMKB(x) ((x) * 1024)
#define MEMMB(x) (MEMKB(x) * 1024)
#define MEMGB(x) (MEMMB(x) * 1024)