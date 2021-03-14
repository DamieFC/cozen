#!/bin/sh
cd ../build
rm -rf cozen.hdd
dd if=/dev/zero bs=1M count=0 seek=8 of=cozen.hdd
parted -s cozen.hdd mklabel msdos
parted -s cozen.hdd mkpart primary 1 100%
../thirdparty/echfs/echfs-utils -m -p0 cozen.hdd quick-format 4096
../thirdparty/echfs/echfs-utils -m -p0 cozen.hdd import kernel.elf build/kernel.elf
../thirdparty/echfs/echfs-utils -m -p0 cozen.hdd import ../limine.cfg limine.cfg
../thirdparty/limine/limine-install cozen.hdd
