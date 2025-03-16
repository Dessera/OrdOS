#pragma once

#include "kernel/task/task.h"

void
init_tss(void);

void
tss_update_esp(struct task* task);