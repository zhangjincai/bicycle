#!/bin/sh
export TERM=vt100
export TERMINFO=/usr/share/terminfo

if [ -f /mnt/firmware/iftop ] ;
then
        echo "run iftop..."
        cd /mnt/firmware/;./iftop -i ppp0 -P -B &
fi

