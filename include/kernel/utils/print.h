#pragma once

#include "kernel/types.h"

size_t
kputs(const char* str);

size_t
kputchar(u8 c);

void
kscrclear(void);

void
kscrscroll(size_t rows);

u16
kget_cursor(void);

void
kset_cursor(u16 cur);