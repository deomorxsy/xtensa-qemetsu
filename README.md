## lumenssif
> simple presence-based lighting control with esp32

Directory structure:
- “platformio.ini” (Project Configuration File)
- src directory where you should place source code (*.h, *.c, *.cpp, *.S, *.ino, etc.)
- lib directory can be used for the project specific (private) libraries. More details are located in lib/README file.
- Miscellaneous files for VCS and Continuous Integration support.

Project environment:
- [nvim-platformio](https://github.com/anurag3301/nvim-platformio.lua/) lua plugin, a  PlatformIO wrapper for neovim written in lua
- [qemu-esp-xtensa](https://github.com/espressif/qemu), a fork of QEMU with Espressif patches.

### Boot on QEMU VM

> I used the Arch distro while doing this, hence the PKGBUILD, but the software build is distro-agnostic, so you can just clone the repo and build manually.

Choose between [```qemu-system-xtensa```](https://github.com/qemu/qemu) (mainline qemu, where you configure with the xtensa option) and [```qemu-esp-xtensa```](https://github.com/espressif/qemu) (the fork).

I choose the second one, then got the PKGBUILD:
```sh
; wget -O ./PKGBUILD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=qemu-esp-xtensa-git
# disable asan, AddressSanitizer
; sed -e "/--enable-sanitizers/{h;d;}" -e "/--disable-gtk/{G;}" -e 's/\(--enable-sanitizers\)/#\1/g' -e 's/disable-gtk/& \\ /' ./PKGBUILD > ./PKGBUILD
; makepkg
# copy package to /opt/ and create a symbolic link of the binary
# to /usr/bin making it available system-wide
; sudo cp -r ./pkg/qemu-esp-xtensa-git/opt/qemu-esp-xtensa-git/ /opt/
; sudo ln -s ./opt/qemu-esp-xtensa-git/bin/qemu-system-xtensa /usr/bin/
```

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
