#!/bin/sh

echo "start make fw ubifs images"

VERSTRING=fw_$1
SUFFIX=P
S_VERSTRING=

#firmware version
rm ../firmware/fw_ver.txt
echo $1 >> ../firmware/fw_ver.txt

chmod 777 ../firmware/ *

#make file rootfs ubifs
mkfs.ubifs -d ../firmware -e 0x1f800 -c 512 -m 0x800 -x lzo -o $VERSTRING

#pack firmware add header md5
./PackFirmware ${VERSTRING}

#modify suffix 'P'
S_VERSTRING=${VERSTRING}${SUFFIX}

#move file to "images" directory
mv $S_VERSTRING ../images/






