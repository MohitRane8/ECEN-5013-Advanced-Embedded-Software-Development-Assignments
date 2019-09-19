# !/bin/bash
# Script to install rootfs.
# Author: Mohit Rane.

set -e
set -u

OUTDIR=/tmp/ecen5013

if [ $# -lt 1 ]
then
	echo -e "\nUSING DEFAULT DIRECTORY ${OUTDIR}\n"
else
	OUTDIR=$1
	echo -e "\nUSING ${OUTDIR} DIRECTORY\n"
fi

if [ -d "$OUTDIR" ]
then
	echo -e "\nDIRECTORY ALREADY EXISTS\n"
else
	exit 1
fi


cd "$OUTDIR"

#5a - Making rootfs
if [ ! -d "$OUTDIR/rootfs" ]
then
	mkdir $OUTDIR/rootfs
	cd $OUTDIR/rootfs
	mkdir bin dev etc home lib proc sbin sys tmp usr var
	mkdir usr/bin usr/lib usr/sbin
	mkdir -p var/log
fi

sudo chown -R root:root *

#5b - Busybox
git clone git://busybox.net/busybox.git
cd busybox
git checkout 1_31_stable

make distclean
make defconfig

#PROBLEM AT THIS STEP: need to add path to sudo
#sudo -i
#PATH=$PATH:/home/mohit/x-tools/arm-unknown-linux-gnueabi/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
sudo env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- CONFIG_PREFIX=$OUTDIR/rootfs install

#5c - installing libraries
cd $OUTDIR/rootfs
arm-unknown-linux-gnueabi-readelf -a bin/busybox | grep "program interpreter"
arm-unknown-linux-gnueabi-readelf -a bin/busybox | grep "Shared library"
arm-unknown-linux-gnueabi-gcc -print-sysroot
export SYSROOT=$(arm-unknown-linux-gnueabi-gcc -print-sysroot)
cd $SYSROOT
ls -l lib/ld-linux.so.3

cd $OUTDIR/rootfs
sudo cp -a $SYSROOT/lib/ld-linux.so.3 lib
sudo cp -a $SYSROOT/lib/ld-2.29.so lib
sudo cp -a $SYSROOT/lib/libc.so.6 lib
sudo cp -a $SYSROOT/lib/libc-2.29.so lib
sudo cp -a $SYSROOT/lib/libm.so.6 lib
sudo cp -a $SYSROOT/lib/libm-2.29.so lib
sudo cp -a $SYSROOT/lib/libresolv.so.2 lib
sudo cp -a $SYSROOT/lib/libresolv-2.29.so lib

#5d - add device nodes
cd $OUTDIR/rootfs
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1
ls -l dev