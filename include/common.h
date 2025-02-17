#pragma once

#define __KSTRINGIFY(x) #x
#define KSTRINGIFY(x) __KSTRINGIFY(x)

#define __KCAT(x, y) x##y
#define KCAT(x, y) __KCAT(x, y)
