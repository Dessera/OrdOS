#pragma once

#define TIMER_INPUT_FREQ 1193180

#define TIMER_CMD_BASE 0x40

#define timer_cmd(n) (TIMER_CMD_BASE + (n))

#define SYS_CLK_RW(rwh, rwl) ((rwl) | ((rwh) << 1))
#define SYS_CLK_RWL SYS_CLK_RW(0, 1)
#define SYS_CLK_RWH SYS_CLK_RW(1, 0)
#define SYS_CLK_RWLH SYS_CLK_RW(1, 1)

#define SYS_CLK_MODE_IRTC 0
#define SYS_CLK_MODE_ONESHOT 1
#define SYS_CLK_MODE_RATEGEN 2
#define SYS_CLK_MODE_SQUARE 3
#define SYS_CLK_MODE_SWT 4
#define SYS_CLK_MODE_HWT 5