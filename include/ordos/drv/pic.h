/**
 * @file pic.h
 * @author Dessera (dessera@qq.com)
 * @brief PIC driver.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#define PIC_CMD(port) (port)
#define PIC_DATA(port) (port + 1)

#ifdef __ASSEMBLER__

#define PIC_MASTER 0x20
#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE 0xA0
#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_INIT 0x10
#define PIC_ICW4 0x01
#define PIC_MASTEROFFS 0x20
#define PIC_SLAVEOFFS 0x28
#define PIC_8086 0x01
#define PIC_EOI 0x20

#else

#include "ordos/lib/types.h"

enum pic_io_port : u16
{
  PIC_MASTER = 0x20,
  PIC_MASTER_CMD = 0x20,
  PIC_MASTER_DATA = 0x21,
  PIC_SLAVE = 0xA0,
  PIC_SLAVE_CMD = 0xA0,
  PIC_SLAVE_DATA = 0xA1,
};

enum pic_flag : u8
{
  PIC_INIT = 0x10,
  PIC_ICW4 = 0x01,
  PIC_MASTEROFFS = 0x20,
  PIC_SLAVEOFFS = 0x28,
  PIC_8086 = 0x01,
  PIC_EOI = 0x20,
};

/**
 * @brief Set PIC mask.
 *
 * @param mask Master and slave mask (master << 8 + slave).
 */
void
pic_set_mask(u16 mask);

#endif
