/*
 * setup.ld
 *
 * Linker script for the i386 setup code
 */
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
  .bstext : AT(0) { *(.bstext) }
  .bsdata : { *(.bsdata) }

  .ldrtext 0 : AT(0x200) { *(.ldrtext) }
  .ldrdata : { *(.ldrdata) }

  /DISCARD/	: {
    *(.note*)
  }
}