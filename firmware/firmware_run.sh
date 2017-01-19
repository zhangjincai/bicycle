#!/bin/sh

#run gpio
#./gpio_set.sh
#sleep 1

insmod /mnt/firmware/gpio_ctrl.ko
#sleep 1

#run watchdog
#./watchdog 
/mnt/firmware/watchdog &


#run logdb
#./logdb
#/mnt/firmware/logdb
#sleep 1

#run gprs

#run wireless app
#./wireless
/mnt/firmware/wireless &
#sleep 1

#run upgrade //
/mnt/firmware/upgrade_app &

#set usb serial





















