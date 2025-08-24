#include "ordos/kernel/driver/pic.h"
#include "ordos/kernel/config.h"
#include "ordos/lib/asm.h"

void
init_pic(void)
{
  outb(PIC_MASTER_CMD, PIC_INIT);
  outb(PIC_MASTER_DATA, 0x20); // NOLINT
  outb(PIC_MASTER_DATA, 0x04);
  outb(PIC_MASTER_DATA, 0x01);

  outb(PIC_SLAVE_CMD, PIC_INIT);
  outb(PIC_SLAVE_DATA, 0x28); // NOLINT
  outb(PIC_SLAVE_DATA, 0x02);
  outb(PIC_SLAVE_DATA, 0x01);

  outb(PIC_MASTER_DATA, ORDOS_DEV_PIC_MASTER_MASK);
  outb(PIC_SLAVE_DATA, ORDOS_DEV_PIC_SLAVE_MASK);
}
