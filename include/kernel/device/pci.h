#pragma once

#define PCI_MASTER_CMD 0x20
#define PCI_MASTER_DATA 0x21
#define PCI_SLAVE_CMD 0xa0
#define PCI_SLAVE_DATA 0xa1

void
init_pci(void);