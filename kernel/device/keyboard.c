#include "kernel/device/keyboard.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/print.h"

static void
__keyboard_handler(u32 irq)
{
  (void)irq;

  u8 data = inb(KEYBORAD_PORT_DATA);

  kprintln("%x", data);
}

void
init_keyboard(void)
{
  intr_register_handler(0x21, __keyboard_handler);
}