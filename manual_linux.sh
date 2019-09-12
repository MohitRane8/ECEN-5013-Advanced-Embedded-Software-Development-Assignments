# !/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u


OUTDIR=/tmp/ecen5013
REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
#DIR is the path where writer executable, finder.sh and tester.sh are stored.
#Refer: https://www.electrictoolbox.com/bash-script-directory/
DIR="$( cd "$(dirname "$0")" ; pwd -P )"



if [ $# -lt 1 ]
then
	echo -e "\nUSING DEFAULT DIRECTORY ${OUTDIR} TO STORE FILES\n"
else
	OUTDIR=$1
	echo -e "\n USING ${OUTDIR} DIRECTORY TO STORE FILES\n"
fi

if [ -d "$OUTDIR" ]
then
	echo -e "\nDIRECTORY ALREADY EXISTS\n"
#	rm -rf "$OUTDIR"/*
else
	mkdir -p "$OUTDIR"
	
	if [ -d "$OUTDIR" ]
	then
		echo -e "\n${OUTDIR} CREATED\n"
	else
		echo -e "\nCANNOT CREATE ${OUTDIR}\n"
	exit 1
	fi
fi



#Installing Dependencies
echo -e "\nINSTALLING THE REQUIRED DEPENDENCIES FOR THE INSTALLATION\n"
"${DIR}"/dependencies.sh


#TODO: Call a script to perform this function here or place a logic inline
#Installing Linux Kernel
echo -e "\nINSTALLING LINUX KERNEL\n"


#TODO: Call a script to perform this function here or place a logic inline
#Installing Rootfs using Busybox
echo -e "\nINSTALLING ROOTFS USING BUSYBOX\n"


#TODO: Call a script to perform this function here or place a logic inline
#Copying writer executable, tester.sh and finder.sh here
echo -e "\nCOPYING WRITER EXECUTABLE, TESTER.SH AND FINDER.SH TO ${OUTDIR}/rootfs/bin\n"


#TODO: Call a script to perform this function here or place a logic inline
#Creating Standalone initramfs.
echo -e "\nCREATING STANDALONE INITRAMFS\n"


#Booting the kernel.
echo -e "\nBOOTING THE KERNEL\n"
"${DIR}"/start_qemu_terminal.sh "$OUTDIR"

