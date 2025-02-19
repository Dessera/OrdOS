# Bootsec in GNU AS

使用`gas`编译`bootsec(mbr)`的优点是可以复用预处理器，我们整个项目只需要定义一份头文件就可以在`ASM`和`C`中复用。

为了实现这一点，我们需要使用`gcc`而非`as`编译，这样其实也统一了整个项目的编译器和编译参数。

## 简易 bootsec 入口

```asm
#define BOOTSEC 0x07c0

.code16
.section ".bstext", "ax"

  ljmp $BOOTSEC, $_bootsec_start

_bootsec_start:
  /* set up segment registers */
  movw %cs, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  xorw %sp, %sp
  sti
  cld

  jmp .

.org 0x1FE
boot_flag: .word 0xaa55
```