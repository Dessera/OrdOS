#pragma once

#define SYS_CLK_INPUT_FREQ 1193180

#define SYS_CLK_CMD 0x43

#define SYS_CLK_TIMER_BASE 0x40
#define SYS_CLK_TIMER_CMD(n) (SYS_CLK_TIMER_BASE + (n))

#define SYS_CLK_INTR_TIMER 0
#define SYS_CLK_INTR_FREQ 100
#define SYS_CLK_INTR_COUNTER (SYS_CLK_INPUT_FREQ / SYS_CLK_INTR_FREQ)

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

void
init_sys_clk(void);