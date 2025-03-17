#pragma once

#include "lib/types.h"

/**
 * @brief Returns the address of the end of the kernel
 *
 * @return size_t The address of the end of the kernel
 */
uintptr_t
compiler_get_kernel_end(void);