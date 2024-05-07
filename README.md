## lumenssif
> simple presence-based lighting control with esp32

[![Flashing](https://github.com/deomorxsy/xtensa-qemetsu/actions/workflows/ci.yml/badge.svg)](https://github.com/deomorxsy/xtensa-qemetsu/actions/workflows/ci.yml)

Project environment:
- [nvim-platformio](https://github.com/anurag3301/nvim-platformio.lua/) lua plugin, a  PlatformIO wrapper for neovim written in lua
- [qemu-esp-xtensa](https://github.com/espressif/qemu), a fork of QEMU with Espressif patches.

Further tooling details for reproducing are in ```./assets/metadata.txt```, a dump using ```pio project metadata```.

Directory structure:
```sh
; tree -C -L 1
.
├── assets
├── CMakeLists.txt
├── compose.yml
├── deploy.yml
├── Dockerfile
├── include
├── lib             #lib directory can be used for the project specific (private) libraries. More details are located in lib/README file.
├── LICENSE
├── Makefile
├── platformio.ini  #“platformio.ini” (Project Configuration File)
├── README.md
├── scripts
├── src             # src directory where you should place source code (*.h, *.c, *.cpp, *.S, *.ino, etc.)
└── test

```

### Build

> I used Arch Linux while doing this, hence the PKGBUILD, but the software build is distro-agnostic, so you can just clone the repo and build manually.

Choose between [```qemu-system-xtensa```](https://github.com/qemu/qemu) (mainline qemu, where you configure with the xtensa option) and [```qemu-esp-xtensa```](https://github.com/espressif/qemu) (the fork).

I choose the second one, then got the PKGBUILD:
```sh
; wget -O ./PKGBUILD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=qemu-esp-xtensa-git
; # disable asan, AddressSanitizer
; sed -e "/--enable-sanitizers/{h;d;}" -e "/--disable-gtk/{G;}" -e 's/\(--enable-sanitizers\)/#\1/g' -e 's/disable-gtk/& \\ /' ./PKGBUILD > ./PKGBUILD
; makepkg
; # copy package to /opt/ and create a symbolic link of the binary
; # to /usr/bin making it available system-wide
; sudo cp -r ./pkg/qemu-esp-xtensa-git/opt/qemu-esp-xtensa-git/ /opt/
; sudo ln -s ./opt/qemu-esp-xtensa-git/bin/qemu-system-xtensa /usr/bin/
```



### Boot

On neovim, build with ```Piorun build```. After building the application, run it on QEMU:

```sh
cat << "EOF" > ./scripts/qemu-xtensa-myifup.sh
#!/bin/sh
#
# qemu-esp-xtensa-git.git
qemu-system-xtensa -nographic \
    -machine esp32 \
    -drive file=./.pio/build/upesy_wroom/firmware.bin,if=mtd,format=raw
EOF

chmod +x ./scripts/qemu-xtensa-myifup.sh
./scripts/qemu-xtensa-myifup.sh
```

### Troubleshooting

Now you may come across this error:
```
qemu-system-xtensa: Error: only 2, 4, 8, 16 MB flash images are supported
```

To get the firmware binary working, use espressif's [esptool](https://github.com/espressif/esptool) to flash the firmware into a binary image containing the second stage bootloader.


Create a python's virtualenv to store the esptool dependencies using [get-esptool.sh](./scripts/get-esptool.sh):
```sh
; cd ./assets/ || return
; git clone git@github.com:espressif/esptool.git
; python3 -m venv venv
; cd - || return
; . ./assets/venv/bin/activate
; pip3 install --upgrade pip
; pip3 install -r ./assets/requirements.txt
; deactivate
```

Flash the second stage bootloader into the image then boot it into QEMU using [flash.sh](./scripts/flash.sh):
```sh
cat << "EOF" > ./scripts/flash.sh
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
EOF

chmod +x ./scripts/flash.sh
./scripts/flash.sh
```
