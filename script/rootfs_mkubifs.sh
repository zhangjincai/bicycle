#!/bin/sh

echo "start make rootfs ubifs images"

VERSTRING=rootfs_$1
SUFFIX=P
S_VERSTRING=

#firmware version
rm ../rootfs/etc/rootfs_ver.txt
echo $1 >> ../rootfs/etc/rootfs_ver.txt

#make file rootfs ubifs
mkfs.ubifs -d ../rootfs -e 0x1f800 -c 640 -m 0x800 -x lzo -o $VERSTRING

#pack firmware add header md5
./PackFirmware ${VERSTRING}

#modify suffix 'P'
S_VERSTRING=${VERSTRING}${SUFFIX}

#move file to "images" directory
mv $S_VERSTRING ../images




