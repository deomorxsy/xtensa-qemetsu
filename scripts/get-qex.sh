#!/usr/sh
wget -O ./PKGBUILD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=qemu-esp-xtensa-git
# disable asan, AddressSanitizer
sed -e "/--enable-sanitizers/{h;d;}" -e "/--disable-gtk/{G;}" -e 's/\(--enable-sanitizers\)/#\1/g' -e 's/disable-gtk/& \\ /' ./PKGBUILD > ./assets/PKGBUILD
cd ./assets/ || return
makepkg
# copy package to /opt/ and create a symbolic link of the binary
# to /usr/bin making it available system-wide
sudo cp -r ./pkg/qemu-esp-xtensa-git/opt/qemu-esp-xtensa-git/ /opt/
sudo ln -s ./opt/qemu-esp-xtensa-git/bin/qemu-system-xtensa /usr/bin/
cd - || return
