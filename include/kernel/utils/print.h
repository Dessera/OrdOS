#pragma once

#include "kernel/types.h"

size_t
kputs(const char* str);

size_t
kput_u32(u32 num);

size_t
kputchar(u8 c);

void
kscrclear(void);

void
kscrscroll(size_t rows);

u16
kVGA_GET_CURSOR(void);

void
kset_cursor(u16 cur);