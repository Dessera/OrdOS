#pragma once

#include "kernel/stddef.h"

char*
kstrcpy(char* dest, const char* src);

size_t
kstrlen(const char* str);

i32
kstrcmp(const char* str1, const char* str2);

char*
kstrchr(const char* str, const char c);

char*
kstrrchr(const char* str, const char c);

char*
kstrcat(char* dest, const char* src);

size_t
kstrchrs(const char* str, const char c);