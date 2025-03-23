#pragma once

#include "kernel/task/task.h"

/**
 * @brief Initialize the TSS Segment
 */
void
init_tss(void);

/**
 * @brief Update the TSS ESP0
 *
 * @param task The task to update the ESP0 for
 */
void
tss_update_esp(struct task* task);