#pragma once

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD 0xa0
#define PIC_SLAVE_DATA 0xa1

#define PIC_EOI 0x20

#ifndef __ASM__

void
init_pci(void);

#endif /* __ASM__ */