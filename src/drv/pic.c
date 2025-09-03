#include "ordos/drv/pic.h"
#include "ordos/config.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/types.h"
#include "ordos/module.h"

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
  outb(PIC_MASTER_DATA, (mask >> BYTE) & BYTEMASK);
  outb(PIC_SLAVE_DATA, mask & BYTEMASK);
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

module_init_noexit(drv_pic, MOD_CORE, pic_entry);
