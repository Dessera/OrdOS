#pragma once

#include "kernel/types.h"

void
kmemcpy(void* dest, const void* src, size_t n);

void
kmemset(void* dest, u8 c, size_t n);