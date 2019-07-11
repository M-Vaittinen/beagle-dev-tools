#!/bin/bash

/bin/pingaa.sh 192.168.1.1


SUCCESS=$?
IFUPDELAY=$1

if [ $SUCCESS -eq 0 ]
then
	echo "YaY WLAN0 UP"
else
	echo "WLAN0 not UP..."
	i=0

	RFKILL=`cat /sys/class/rfkill/rfkill0/soft`
	if [ $RFKILL -eq 0 ]
	then
		echo "RFKILL seems to be off"
	else
		echo "RFKILL on - trying to disable"
		echo 0 > /sys/class/rfkill/rfkill/soft
		/sbin/ifdown wlan0
		sleep 1
		/sbin/ifup wlan0
		sleep $IFUPDELAY
		/bin/pingaa.sh 192.168.1.1
	fi

	echo "going to while"

	while [[ $i -lt 4 && $SUCCESS -ne 0 ]]
	do
		echo "upsetting WLAN0..."
		/usr/bin/pkill wpa_supplicant

		sleep 1
		/sbin/ifup wlan0
		sleep $IFUPDELAY

		/bin/pingaa.sh 192.168.1.1

		SUCCESS=$?
		i=$(($i+1))

		if [ $SUCCESS -eq 0 ]
		then
			echo "got WLAN0 UP"
		else
			/sbin/ifdown wlan0
			echo "WLAN0 still not UP ... retrying $i"
		fi
	done
fi


