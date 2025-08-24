#include "ordos/kernel/driver/vga.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

void
vga_set(vga_cursor_t cursor, u16 value)
{
  if (cursor > VGA_DP_SIZE) {
    return;
  }

  u16* pdata = poffset(VGA_BUF_ADDR, cursor * 2);
  *pdata = value;
}

u16
vga_get(vga_cursor_t cursor)
{
  if (cursor > VGA_DP_SIZE) {
    return 0;
  }

  u16* pdata = poffset(VGA_BUF_ADDR, cursor * 2);
  return *pdata;
}

void
vga_set_cursor(vga_cursor_t cursor)
{
  if (cursor > VGA_DP_SIZE) {
    return;
  }

  u8 cursor_hb = (cursor >> BYTE);
  u8 cursor_lb = (cursor & BYTEMASK);
  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_HIGH);
  outb(VGA_PORT_DATA, cursor_hb);
  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_LOW);
  outb(VGA_PORT_DATA, cursor_lb);
}

vga_cursor_t
vga_get_cursor(void)
{
  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_HIGH);
  u8 cursor_hb = inb(VGA_PORT_DATA);

  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_LOW);
  u8 cursor_lb = inb(VGA_PORT_DATA);

  return ((u16)cursor_hb << BYTE) | (u16)cursor_lb;
}

void
vga_clear(void)
{
  for (size_t i = 0; i < VGA_DP_SIZE; i++) {
    vga_set(i, VGA_BLANK);
  }

  vga_set_cursor(0);
}

void
vga_scroll(size_t offs)
{
  if (offs == 0) {
    return;
  }

  if (offs >= VGA_BUF_HEIGHT) {
    vga_clear();
    return;
  }

  for (size_t i = 0; i < (VGA_BUF_HEIGHT - offs) * VGA_BUF_WIDTH; i++) {
    vga_set(i, vga_get(i + (offs * VGA_BUF_WIDTH)));
  }

  for (size_t i = (VGA_BUF_HEIGHT - offs) * VGA_BUF_WIDTH; i < VGA_DP_SIZE;
       i++) {
    vga_set(i, VGA_BLANK);
  }
}
