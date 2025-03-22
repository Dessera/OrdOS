#pragma once

#include "lib/types.h"

size_t
puts(const char* str);

size_t
print(const char* fmt, ...);

size_t
println(const char* fmt, ...);

size_t
sprint(char* buf, const char* fmt, ...);

size_t
vsprint(char* buf, const char* fmt, VA_LIST args);