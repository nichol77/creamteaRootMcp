#/bin/bash
/usr/sbin/setenforce 0
BUS=`/sbin/lsusb | grep 6672:2920 | awk '{ print $2 }'`
DEVICE=`/sbin/lsusb | grep 6672:2920 | awk '{ print $4 }' | awk -F: '{ print $1 }'`
chmod a+rw /dev/bus/usb/${BUS}/${DEVICE}
chmod a+rw /dev/usbdev*
