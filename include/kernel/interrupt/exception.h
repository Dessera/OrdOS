#pragma once

#include "lib/types.h"

ENUM_DECLARE(intr_exception,
             IE_ZERO_DIVIDE,
             IE_DEBUG,
             IE_NMI,
             IE_BREAKPOINT,
             IE_OVERFLOW,
             IE_BOUND_RANGE,
             IE_INVALID_OPCODE,
             IE_DEVICE_NOT_AVAILABLE,
             IE_DOUBLE_FAULT,
             IE_COSEGMENT_OVERLAP,
             IE_INVALID_TSS,
             IE_SEGMENT_NOT_PRESENT,
             IE_STACK_FAULT,
             IE_GENERAL_PROTECTION,
             IE_PAGE_FAULT,
             IE_RESERVED,
             IE_X87_FLOATING_POINT_ERROR,
             IE_ALIGNMENT_CHECK,
             IE_MACHINE_CHECK,
             IE_SIMD_FLOATING_POINT_ERROR);

/**
 * @brief Register exception handler
 */
void
init_exception(void);
