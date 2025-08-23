QEMU := "qemu-system-i386"

BUILDDIR := "builddir"
TARGET := BUILDDIR + "/boot.img"

default:
  just -l

configure:
  meson setup {{BUILDDIR}} --reconfigure --wipe

build:
  cd {{BUILDDIR}} && meson compile

debug:
  {{QEMU}} -drive format=raw,file={{TARGET}} -s -S

run:
  {{QEMU}} -drive format=raw,file={{TARGET}}
