#!/bin/sh

#set eth0
#ifconfig eth0 192.168.2.169 netmask 255.255.255.0
#route add default gw 192.168.2.1 eth0
ifconfig eth0 up

#set eth1
#ifconfig eth1 192.168.1.169 netmask 255.255.255.0
#route add default gw 192.168.1.1 eth1
ifconfig eth1 up

#set lo
ifconfig lo 127.0.0.1
ifconfig lo up

#set usb serial

#run bicycle app //
/mnt/app/bicycle &
sleep 1

#run QT //
/mnt/app/bicycle_gui -qws -nomouse &
#sleep 1


#run dog soft //
/mnt/app/dog_soft &


#run logclient //
/mnt/app/logclient &
























