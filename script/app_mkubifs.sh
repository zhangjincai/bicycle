#!/bin/sh

echo "start make app ubifs images"

VERSTRING=app_$1
SUFFIX=P
S_VERSTRING=

#firmware version
rm ../application/appl_ver.txt
echo $1 >> ../application/appl_ver.txt

chmod 777 ../application/ *

#make file rootfs ubifs
mkfs.ubifs -d ../application -e 0x1f800 -c 512 -m 0x800 -x lzo -o $VERSTRING

#pack firmware add header md5
./PackFirmware ${VERSTRING}

#modify suffix 'P'
S_VERSTRING=${VERSTRING}${SUFFIX}

#move file to "images" directory
mv $S_VERSTRING ../images/






