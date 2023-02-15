#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/jurasos.kernel isodir/boot/jurasos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=1

menuentry "jurasos" {
	multiboot2 /boot/jurasos.kernel
	boot
}
EOF
grub-mkrescue -o jurasos.iso isodir