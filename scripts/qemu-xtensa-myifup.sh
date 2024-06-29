#!/bin/sh
#
# qemu-esp-xtensa-git.git
qemu-system-xtensa -nographic \
    -machine esp32 \
    -drive file=./.pio/build/upesy_wroom/firmware.bin,if=mtd,format=raw
    #-drive file=./merged-flash.bin,if=mtd,format=raw \
    #-S -s
