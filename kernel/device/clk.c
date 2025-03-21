#include "kernel/device/clk.h"
#include "kernel/log.h"
#include "lib/asm.h"

#define MK_SYS_CLK_CMD(index, rw, mode) ((u8)(index << 6 | rw << 4 | mode << 1))
#define MK_SYS_CLK_LB(cnt) ((u8)(cnt & 0xff))
#define MK_SYS_CLK_HB(cnt) ((u8)((cnt >> 8) & 0xff))

void
init_clk(void)
{
  outb(SYS_CLK_CMD,
       MK_SYS_CLK_CMD(SYS_CLK_INTR_TIMER, SYS_CLK_RWLH, SYS_CLK_MODE_RATEGEN));
  outb(SYS_CLK_TIMER_CMD(SYS_CLK_INTR_TIMER),
       MK_SYS_CLK_LB(SYS_CLK_INTR_COUNTER));
  outb(SYS_CLK_TIMER_CMD(SYS_CLK_INTR_TIMER),
       MK_SYS_CLK_HB(SYS_CLK_INTR_COUNTER));

  KDEBUG("sys freq: %u ticks/sec", SYS_CLK_INTR_FREQ);
}