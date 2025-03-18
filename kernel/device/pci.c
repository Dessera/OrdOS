#include "kernel/device/pci.h"
#include "kernel/config/device.h"
#include "lib/asm.h"

void
init_pic(void)
{
  outb(PIC_MASTER_CMD, 0x11);
  outb(PIC_MASTER_DATA, 0x20);
  outb(PIC_MASTER_DATA, 0x04);
  outb(PIC_MASTER_DATA, 0x01);

  outb(PIC_SLAVE_CMD, 0x11);
  outb(PIC_SLAVE_DATA, 0x28);
  outb(PIC_SLAVE_DATA, 0x02);
  outb(PIC_SLAVE_DATA, 0x01);

  outb(PIC_MASTER_DATA, DEVICE_PIC_MASTER_MASK);
  outb(PIC_SLAVE_DATA, DEVICE_PIC_SLAVE_MASK);
}
