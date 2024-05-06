#!/bin/sh
#
# qemu-esp-xtensa-git.git
qemu-system-xtensa -nographic \
    -machine esp32 \
    -drive file=./.pio/build/upesy_wroom/firmware.elf,if=mtd,format=raw
