#!/bin/bash

if [ $# -ne 3 ]; then
  echo "Usage: $0 <kernel_path> <cfg_path> <output_path>"
  exit 1
fi

kernel_path=$1
cfg_path=$2
output_iso=$3
temp_dir="iso"

if [ ! -f "$kernel_path" ]; then
  echo "Error: Kernel $kernel_path does not exist."
  exit 1
fi

if [ ! -f "$cfg_path" ]; then
  echo "Error: Grub config $cfg_path dose not exist."
  exit 1
fi

rm -rf "$temp_dir"

mkdir -p "$temp_dir/boot/grub"

cp -v "$kernel_path" "$temp_dir/boot/kernel.bin"
cp -v "$cfg_path" "$temp_dir/boot/grub/grub.cfg"

GRUB_CMD=""
if command -v grub2-mkrescue &> /dev/null; then
  GRUB_CMD="grub2-mkrescue"
elif command -v grub-mkrescue &> /dev/null; then
  GRUB_CMD="grub-mkrescue"
else
  echo "Error: Cannot find grub-mkrescue or grub2-mkrescue"
  exit 1
fi

$GRUB_CMD -o "$output_iso" "$temp_dir"
