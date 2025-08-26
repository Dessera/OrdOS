QEMU := "qemu-system-i386"

BUILDDIR := "builddir"
TARGET := BUILDDIR + "/boot.img"

default:
  @just -l

configure:
  meson setup {{BUILDDIR}} --reconfigure --wipe

build:
  cd {{BUILDDIR}} && meson compile

qemu-debug:
  {{QEMU}} -drive format=raw,file={{TARGET}} -s -S

qemu:
  {{QEMU}} -drive format=raw,file={{TARGET}} --enable-kvm
