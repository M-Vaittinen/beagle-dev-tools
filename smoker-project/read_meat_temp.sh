#!/bin/bash

ADC_FILE=/sys/bus/iio/devices/iio\:device1/in_voltage0_raw
i=0
while [[ $i -lt 20 ]]
do
	TEMPVAL=$(cat "$ADC_FILE")
	RAW_VOLT=$((TEMPVAL+RAW_VOLT))
	i=$((i+1))
	sleep 0.01
done
echo $((RAW_VOLT/20))

