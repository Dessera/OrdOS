#pragma once

#ifndef __ASSEMBLER__
/**
 * @brief Privilege level
 */
enum dpl
{
  DPL_USER = 3,
  DPL_KERNEL = 0
};

enum presence
{
  PRESENT = 1,
  NOT_PRESENT = 0
};

#else
#define DPL_USER 3
#define DPL_KERNEL 0

#define PRESENT 1
#define NOT_PRESENT 0
#endif