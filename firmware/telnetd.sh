#!/bin/sh
#change telnet dufault port
echo "change telnet dufault port..."
killall -9 telnetd
sleep 1
/usr/sbin/telnetd -p 8989 -l /bin/login