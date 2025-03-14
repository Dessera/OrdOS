#pragma once

#include "kernel/types.h"

void*
syscall_common_handler(u32 index, void* arg1, void* arg2);