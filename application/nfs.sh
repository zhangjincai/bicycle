#!/bin/sh

ifconfig eth0 down

#echo "set ip address"
ifconfig eth1 192.168.1.169 netmask 255.255.255.0
ifconfig eth1 up

echo "set default gateway"
route add default gw 192.168.1.1

echo "mount nfs"
#mount -t nfs 192.168.1.200:/home/dengjs/dowork/gzPublicBicycle/release /mnt/mount -o nolock
#mount -t nfs 192.168.1.200:/home/dengjs/dowork/BicycleProject/application /mnt/mount -o nolock

mount -t nfs 192.168.1.200:/home/dengjs/dowork/BicycleProjectV2/application /mnt/mount -o nolock


