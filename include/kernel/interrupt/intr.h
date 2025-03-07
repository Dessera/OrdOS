#pragma once

#include "kernel/types.h"

typedef void (*interrupt_handler_t)(u32);

void
init_intr(void);

void
intr_register_handler(u32 interrupt_number, interrupt_handler_t handler);

bool
intr_get_status(void);

bool
intr_set_status(bool status);

void
intr_common_handler(u32 irq);
