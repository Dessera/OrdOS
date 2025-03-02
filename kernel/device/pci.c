#include "kernel/device/pci.h"
#include "kernel/utils/asm.h"

void
init_pci(void)
{
  outb(PCI_MASTER_CMD, 0x11);
  outb(PCI_MASTER_DATA, 0x20);
  outb(PCI_MASTER_DATA, 0x04);
  outb(PCI_MASTER_DATA, 0x01);

  outb(PCI_SLAVE_CMD, 0x11);
  outb(PCI_SLAVE_DATA, 0x28);
  outb(PCI_SLAVE_DATA, 0x02);
  outb(PCI_SLAVE_DATA, 0x01);
  outb(PCI_MASTER_DATA, 0xfe);
  outb(PCI_SLAVE_DATA, 0xff);
}