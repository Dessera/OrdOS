#pragma once

#include "lib/types.h"

/**
 * @brief Register exception handler
 */
void
init_exception(void);

/**
 * @brief Get exception name
 *
 * @param exception Exception number (interrupt ID)
 * @return char* Exception name
 */
char*
exception_get_name(u32 exception);