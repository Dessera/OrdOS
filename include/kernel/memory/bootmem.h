#pragma once

#include "lib/types.h"

void
init_bootmem(void);

void*
bootmem_alloc(u32 size);

void*
bootmem_exit(void);