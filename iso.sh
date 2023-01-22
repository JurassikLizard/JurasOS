#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/jurasos.kernel isodir/boot/jurasos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "jurasos" {
	multiboot /boot/jurasos.kernel
}
EOF
grub-mkrescue -o jurasos.iso isodir