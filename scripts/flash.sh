#!/usr/bin/sh
#
# source(POSIX sh) venv relative to the repo root directory
. ./assets/venv/bin/activate

./assets/esptool/esptool.py --chip ESP32 merge_bin \
    -o merged-flash.bin \
    --flash_mode dio \
    --flash_size 4MB \
    0x1000 .pio/build/upesy_wroom/bootloader.bin \
    0x8000 .pio/build/upesy_wroom/partitions.bin \
    0x10000 .pio/build/upesy_wroom/firmware.bin \
    --fill-flash-size 4MB

deactivate
