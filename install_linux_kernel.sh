# !/bin/bash
# Script to open qemu terminal.
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

#Cloning the kernel
KERNDIR="linux-stable"
if [ ! -d $KERNDIR ]
then
    git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
    cd $KERNDIR
    git checkout v5.1.10
else
    cd $KERNDIR
fi

#Building kernel for QEMU
make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- mrproper
make ARCH=arm versatile_defconfig
make -j4 ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- zImage
make -j4 ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- modules
make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi- dtbs

#Copying resulting generated files to output directory
cp -a $OUTDIR/$KERNDIR/. $OUTDIR/