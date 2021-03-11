# Build guide

## Table of contents
- Building cozenOS
    -   [Dependencies](#dependencies)
    -   [Getting the source code](#getting-the-source-code)
	-   [Configuring Meson](#configuring)
    -   [Building](#building)
    -   [Running in a virtual machine](#running-in-a-virtual-machine)



## Dependencies

Building cozenOS requires:

- A Linux distribution or any other UNIX-like operating system
- nasm (Assembler)
- LLVM (toolchain)
- GNU/Make (build system)
- Meson (meta-build system)
- Ninja (build system used with meson)
- QEMU, VirtualBox or any other virtualization software

```sh
# On debian-based distributions
$ sudo apt install nasm llvm make meson ninja-build qemu-system-x86 
```

```sh
# on arch-based distributions
$ sudo pacman -S nasm llvm make meson ninja qemu
```

```sh
# on gentoo
$ sudo emerge --ask llvm nasm make meson ninja qemu
```

## Getting the source code

To get the source code, just clone the repository.

```sh
# Github
$ git clone https://github.com/Smart6502/cozen.git --recursive
```

## Configuring
First, you need to configure meson to generate ninja files, so run this command:

```sh
$ cd cozen/scripts/build
$ ./configure.sh
$ cd ../..
```
## Building 

To build cozen, simply run:

```sh
$ make
```

## Running in a virtual machine

Cozen has only been tested on the following virtualization software:
- QEMU
- VirtualBox
- Other VMs might work

To run in QEMU, just execute the following command:

```sh
$ make run
```

To run in VirtualBox, create a new machine with the following properties:

Make sure to run 
```sh
$ make disk
```
to generate the disk image

- Name: cozen (optional)
- Type: other
- Version: Other/Unknown (64 bit)
- Memory size: 64 mb 
- Hard Drive: use an existing virtual hard disk file -> choose the disk.hdd file in the build directory of the cloned repo

Thanks for building cozen! :heart:
