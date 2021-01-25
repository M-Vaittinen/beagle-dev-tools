#!/bin/bash


modprobe maxim_thermocouple

if [ $? -eq 0 ]
then
	echo "maxim_thermocouple probed"
else
	echo "Failed to probe maxim_thermocouple"
fi

insmod /savustin/core/kernel_and_dt/mva_overlay.ko
if [ $? -eq 0 ]
then
	echo "mva_verlay load"
else
	echo "Failed to load mva_verlay"
fi

echo 61 > /sys/class/gpio/export
if [ $? -eq 0 ]
then
	echo "GPIO exported"
else
	echo "Failed to export GPIO"
fi

sleep 0.1
echo out > /sys/class/gpio/gpio61/direction
if [ $? -eq 0 ]
then
	echo "GPIO direction set out"
else
	echo "Failed to set GPIO direction"
fi

sleep 0.1
echo 0 > /sys/class/gpio/gpio61/value
if [ $? -eq 0 ]
then
	echo "GPIO initially driven LOW"
else
	echo "Failed to change GPIO state"
fi

sleep 1

dd if=/savustin/core/kernel_and_dt/max6675.dtbo of=/sys/kernel/mva_overlay/overlay_add bs=4M
if [ $? -eq 0 ]
then
	echo "DT overlay for maxim max6675 load"
else
	echo "Failed to load DT overlay for maxim max6675"
fi

dd if=/savustin/core/kernel_and_dt/bbb_analog_inputs.dtbo of=/sys/kernel/mva_overlay/overlay_add bs=4M
if [ $? -eq 0 ]
then
	echo "DT overlay for maxim beagle bone black AIN load"
else
	echo "Failed to load DT overlay for beagle bone black AIN"
fi

sleep 1
