#include "kernel/device/sys_clk.h"
#include "kernel/assert.h"
#include "kernel/config/task.h"
#include "kernel/interrupt/intr.h"
#include "kernel/task/task.h"
#include "kernel/task/thread.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/print.h"

#define MK_SYS_CLK_CMD(index, rw, mode) ((u8)(index << 6 | rw << 4 | mode << 1))
#define MK_SYS_CLK_LB(cnt) ((u8)(cnt & 0xff))
#define MK_SYS_CLK_HB(cnt) ((u8)((cnt >> 8) & 0xff))

static void
__init_sys_intr_clk(void)
{
  outb(SYS_CLK_CMD,
       MK_SYS_CLK_CMD(SYS_CLK_INTR_TIMER, SYS_CLK_RWLH, SYS_CLK_MODE_RATEGEN));
  outb(SYS_CLK_TIMER_CMD(SYS_CLK_INTR_TIMER),
       MK_SYS_CLK_LB(SYS_CLK_INTR_COUNTER));
  outb(SYS_CLK_TIMER_CMD(SYS_CLK_INTR_TIMER),
       MK_SYS_CLK_HB(SYS_CLK_INTR_COUNTER));
}

static void
__sys_clk_handler(u32 irq)
{
  (void)irq;

  struct task_t* task = thread_current();

  KASSERT_MSG(task->tmagic == TASK_MAGIC,
              "sys_clk_handler: invalid task object");

  task->elapsed_ticks++;

  if (task->ticks > 0) {
    task->ticks--;
  } else {
    // bool old_status = intr_set_status(false);
    thread_scheduler();
    // intr_set_status(old_status);
  }
}

void
init_sys_clk(void)
{
  kputs("Initializing system clock...\n");

  __init_sys_intr_clk();
  intr_register_handler(0x20, __sys_clk_handler);
}