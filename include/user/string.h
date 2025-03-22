#pragma once

#include "lib/types.h"

char*
strcpy(char* dest, const char* src);

size_t
strlen(const char* str);

ssize_t
strcmp(const char* str1, const char* str2);

char*
strchr(const char* str, const char c);

char*
strrchr(const char* str, const char c);

char*
strcat(char* dest, const char* src);

size_t
strchrs(const char* str, const char c);

void
memcpy(void* dest, const void* src, size_t n);

void
memset(void* dest, u8 c, size_t n);

ssize_t
memcmp(const void* s1, const void* s2, size_t n);