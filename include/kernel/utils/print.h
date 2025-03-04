#pragma once

#include "kernel/types.h"

void
kputs(const char* str);

void
kput_u32(u32 num);

void
kputchar(char c);

void
kscrclear(void);

void
kscrscroll(size_t rows);

u16
kget_cursor(void);

void
kset_cursor(u16 cur);