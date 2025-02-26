#pragma once

#include "kernel/screen.h"

_pprint: # params : %esi = string, %eax = line
  pusha

  movl $SCREEN_WIDTH, %ecx
  mull %ecx
  movl $SCREEN_BUFFER_ADDR, %edx
  # get real mem position (mul 160 to eax)
  addl %eax, %edx

  movl $0x0, %ecx
_pprint_loop:
  lodsb
  andb %al, %al
  jz _pprint_done
  movb $0x0f, %ah
  movw %ax, (%edx, %ecx, 2)
  incl %ecx
  jmp _pprint_loop
_pprint_done:
  popa
  ret