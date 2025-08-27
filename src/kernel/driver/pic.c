#include "ordos/kernel/driver/pic.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/error.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/module.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/types.h"

static void
__pic_init(u16 port, u8 offs, u8 bind)
{
  outb(PIC_CMD(port), PIC_INIT | PIC_ICW4);
  outb(PIC_DATA(port), offs);
  outb(PIC_DATA(port), bind);
  outb(PIC_DATA(port), PIC_8086);
}

void
pic_set_mask(u16 mask)
{
  outb(PIC_MASTER_CMD, (mask >> BYTE) & BYTEMASK);
  outb(PIC_SLAVE_CMD, mask & BYTEMASK);
}

int
pic_entry(struct module* mod)
{
  __pic_init(PIC_MASTER, PIC_MASTEROFFS, 0x04);
  __pic_init(PIC_SLAVE, PIC_SLAVEOFFS, 0x02);

  pic_set_mask(ORDOS_DEV_PIC_MASK);

  kinfo("%s: PIC initialized with mask %x", mod->name, ORDOS_DEV_PIC_MASK);
  return E_SUCCESS;
}

module_init_noexit(sys_pic, MOD_COREMOD, pic_entry);
