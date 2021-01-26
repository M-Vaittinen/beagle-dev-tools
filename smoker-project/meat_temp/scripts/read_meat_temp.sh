#!/bin/bash
#
# Read the BBB ADC value via sysfs.
# The first smoker prototype did not have capacitor to
# filter/stabilize ADC voltage so measurements did vary _a lot_.
# problem was worked around by reading 20 measurements in a loop and
# computing the average of measurements.


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

