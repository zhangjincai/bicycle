#!/bin/sh

#echo "Kill wireless_fo_4g"
#killall -9 wireless_for_4g

#echo "Run wireless_for_4g"
#./wireless_for_4g &

/mnt/firmware/watchdog &
/mnt/firmware/upgrade_app &
./wireless_4g &

sleep 1

./bicycle &
#sleep 2
#./bicycle_gui -qws -nomouse &


