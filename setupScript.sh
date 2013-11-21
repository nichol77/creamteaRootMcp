#/bin/bash
/usr/sbin/setenforce 0
BUS_LIST=`/sbin/lsusb | grep 6672:2920 | awk '{ print $2 }'`
DEVICE_LIST=`/sbin/lsusb | grep 6672:2920 | awk '{ print $4 }' | awk -F: '{ print $1 }'`
#echo $BUS_LIST 
#echo $DEVICE_LIST

NUM_LINES=`/sbin/lsusb | grep -c 6672:2920`
echo "Found $NUM_LINES Cypress EZ-USB devices"

BUSCOUNT=1
for BUS in $BUS_LIST; do
    BUS_ARRAY[$BUSCOUNT]=$BUS
    let "BUSCOUNT = BUSCOUNT +1"
done

DEVICECOUNT=1
for DEVICE in $DEVICE_LIST; do
    DEVICE_ARRAY[$DEVICECOUNT]=$DEVICE
    let "DEVICECOUNT = DEVICECOUNT +1"
done

for INDEX in `seq 1 $NUM_LINES`; do
    BUS=${BUS_ARRAY[$INDEX]}
    DEVICE=${DEVICE_ARRAY[$INDEX]}
    echo $BUS $DEVICE
    chmod a+rw /dev/bus/usb/${BUS}/${DEVICE}
done

chmod a+rw /dev/usbdev*
