#!/bin/bash


###### Definitions
START_DELAY=80
#Seconds to sleep between measurements
INTERVAL=5
#Heating rage - Heating stops when measured temp exceeds
#TARGET_TEMP + TEMPERATURE_AREA/2 and starts again when
#temperature drops below TARGET_TEMP - TEMPERATURE_AREA/2
TEMPERATURE_AREA=1
#ADC_FILE=/sys/bus/iio/devices/iio\:device1/in_voltage0_raw
ADC_FILE=/read_meat_temp.sh
TEMP=/sys/bus/spi/devices/spi0.0/iio\:device0/in_temp_raw
CTRL_FILE=/sys/class/gpio/gpio61/value
TARGET_TEMP=`cat /target_temp.txt`
LOWER_LIMIT=$(($TARGET_TEMP-$TEMPERATURE_AREA/2))
UPPER_LIMIT=$(($TARGET_TEMP+$TEMPERATURE_AREA/2))



##### Intial settings
#Initialize logs
echo "" > /var/www/html/oven.log
echo "=================================================" >> /oven.log
echo "" >> /var/www/html/oven.log
echo "" >> /var/www/html/oven.log
/bin/date >> /var/www/html/oven.log

echo "waiting $START_DELAY seconds to get rt-driver up" >> /var/www/html/oven.log

#Wait a minute to ensure rt-wlan driver is UP.
sleep $START_DELAY
echo Oven Temp started >> /var/www/html/oven.log

#Upset WLAN0
/prepare_wlan.sh 15 >> /var/www/html/oven.log

/bin/pingaa.sh 192.168.1.1

if [ $? -eq 0 ]
then
	echo "pinging 192.168.1.1 succeeded" >> /var/www/html/oven.log
else
	echo "WLAN0 not up?"
fi

echo "target temperature $TARGET_TEMP" >> /var/www/html/oven.log
echo "Range $LOWER_LIMIT ... $UPPER_LIMIT"

CYCLE=1

# Prepare thermocouple, Analog inputs and GPIO. 
/setup_spi_and_gpio.sh

mv /temps.txt /temps.old
mv /meat_temp.txt /meat_temp.old


############ Start the main loop measuring oven and meat temperatures

for ((;;))
do
	#re-read target temp in case php has changed it
	TARGET_TEMP=`cat /target_temp.txt`
	LOWER_LIMIT=$(($TARGET_TEMP-$TEMPERATURE_AREA/2))
	UPPER_LIMIT=$(($TARGET_TEMP+$TEMPERATURE_AREA/2))

	RAW_VOLT=`$ADC_FILE`
	echo "#ADC val $RAW_VOLT" >> /meat_temp.txt

	MEAT_TEMP=$(/bin/temp $RAW_VOLT)
	echo "Meat Temperature  $MEAT_TEMP, cycle $CYCLE" >> /meat_temp.txt
	echo "$MEAT_TEMP" > /var/www/html/meat_temp

	CURR_TEMP=`cat $TEMP`
	CURR_TEMP=$((CURR_TEMP/4))
	echo "Temperature  $CURR_TEMP, cycle $CYCLE" >> /var/www/html/oven.log
	echo "Temperature  $CURR_TEMP, cycle $CYCLE" >> /temps.txt
	echo "#Target temp $TARGET_TEMP, limits $LOWER_LIMIT ... $UPPER_LIMIT" >> /var/www/html/oven.log

	if [ "$CURR_TEMP" -gt "$UPPER_LIMIT" ]
	then
		echo 1 > $CTRL_FILE
		echo "#Disabling heater" >> /var/www/html/oven.log
	fi
	if [ "$CURR_TEMP" -lt "$LOWER_LIMIT" ]
	then
		echo 0 > $CTRL_FILE
		echo "#Enabling heater" >> /var/www/html/oven.log
	fi

	sleep $INTERVAL
	CYCLE=$((CYCLE+1))
done
echo "YaY - Done" >> /var/www/html/oven.log

