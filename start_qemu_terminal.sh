# !/bin/bash
# Script to open qemu terminal.
# Author: Siddhant Jajoo.

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

#Booting the kernel
echo -e "\nBOOTING THE KERNEL\n"
echo QEMU_AUDIO_DRV=none qemu-system-arm -m 256M -nographic -M versatilepb -kernel zImage -append "console=ttyAMA0 rdinit=/bin/sh" -dtb versatile-pb.dtb -initrd initramfs.cpio.gz
QEMU_AUDIO_DRV=none qemu-system-arm -m 256M -nographic -M versatilepb -kernel zImage -append "console=ttyAMA0 rdinit=/bin/sh" -dtb versatile-pb.dtb -initrd initramfs.cpio.gz
