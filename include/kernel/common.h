#pragma once

#define __KSTRINGIFY(x) #x
#define KSTRINGIFY(x) __KSTRINGIFY(x)

#define __KCAT(x, y) x##y
#define KCAT(x, y) __KCAT(x, y)

#define KMEMKB(x) ((x) * 1024)
#define KMEMMB(x) (KMEMKB(x) * 1024)
#define KMEMGB(x) (KMEMMB(x) * 1024)