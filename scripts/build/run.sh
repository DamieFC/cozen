#!/bin/sh
cd  ../build
qemu-system-x86_64 -machine q35 -drive file=cozen.hdd,format=raw -enable-kvm -serial stdio -rtc base=localtime -m 256M -soundhw pcspk # pcspkr sound

