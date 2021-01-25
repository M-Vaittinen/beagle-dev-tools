#!/bin/bash


###### Definitions
START_DELAY=59
#Seconds to sleep between measurements
INTERVAL=5
#Heating rage - Heating stops when measured temp exceeds
#TARGET_TEMP + TEMPERATURE_AREA/2 and starts again when
#temperature drops below TARGET_TEMP - TEMPERATURE_AREA/2
TEMPERATURE_AREA=1
#ADC_FILE=/sys/bus/iio/devices/iio\:device1/in_voltage0_raw
ADC_FILE=/savustin/bin/read_meat_temp.sh
COEFF_FILE=/savustin/configs/coeff.txt
TEMP=/sys/bus/spi/devices/spi0.0/iio\:device0/in_temp_raw
CTRL_FILE=/sys/class/gpio/gpio61/value
TARGET_TEMP=`cat /savustin/tmp/target_temp.txt`
LOWER_LIMIT=$(($TARGET_TEMP-$TEMPERATURE_AREA/2))
UPPER_LIMIT=$(($TARGET_TEMP+$TEMPERATURE_AREA/2))
LOG_FILE=/var/www/html/oven.log
SAVUSTIN_LAMPO=/savustin/tmp/temps.txt
LIHALAMPO=/savustin/tmp/meat_temp.txt

HEAT_ON=1
HEAT_OFF=0

##### Intial settings
#Initialize logs
echo "" > $LOG_FILE
echo "=================================================" >> $LOG_FILE
echo "" >> $LOG_FILE
echo "" >> $LOG_FILE
/bin/date >> $LOG_FILE

echo "waiting $START_DELAY seconds to get rt-driver up" >> $LOG_FILE

#Wait a minute to ensure rt-wlan driver is UP.
sleep $START_DELAY
echo Oven Temp started >> $LOG_FILE

#Upset WLAN0
/savustin/core/prepare_wlan.sh 15 >> $LOG_FILE

/bin/pingaa.sh 192.168.1.1

if [ $? -eq 0 ]
then
	echo "pinging 192.168.1.1 succeeded" >> $LOG_FILE
else
	echo "WLAN0 not up?"
fi

echo "target temperature $TARGET_TEMP" >> $LOG_FILE
echo "Range $LOWER_LIMIT ... $UPPER_LIMIT"

CYCLE=1

# Prepare thermocouple, Analog inputs and GPIO. 
/savustin/core/setup_spi_and_gpio.sh

mv $SAVUSTIN_LAMPO /savustin/tmp/temps.old
mv $LIHALAMPO /savustin/tmp/meat_temp.old


############ Start the main loop measuring oven and meat temperatures

for ((;;))
do
	#re-read target temp in case php has changed it
	TARGET_TEMP=`cat /savustin/tmp/target_temp.txt`
	LOWER_LIMIT=$(($TARGET_TEMP-$TEMPERATURE_AREA/2))
	UPPER_LIMIT=$(($TARGET_TEMP+$TEMPERATURE_AREA/2))

	RAW_VOLT=`$ADC_FILE`
	echo "#ADC val $RAW_VOLT" >> $LIHALAMPO

	MEAT_TEMP=$(/savustin/bin/temp $RAW_VOLT $COEFF_FILE)
	echo "Meat Temperature  $MEAT_TEMP, cycle $CYCLE" >> $LIHALAMPO
	echo "$MEAT_TEMP" > /var/www/html/meat_temp

	CURR_TEMP=`cat $TEMP`
	CURR_TEMP=$((CURR_TEMP/4))
	echo "Temperature  $CURR_TEMP, cycle $CYCLE" >> $LOG_FILE
	echo "Temperature  $CURR_TEMP, cycle $CYCLE" >> $SAVUSTIN_LAMPO
	echo $CURR_TEMP > /var/www/html/temp
	echo "#Target temp $TARGET_TEMP, limits $LOWER_LIMIT ... $UPPER_LIMIT" >> $LOG_FILE

	FORCE=`cat /savustin/tmp/forcevalue`

	if [ "$FORCE" -eq 0 ]
	then
		if [ "$CURR_TEMP" -gt "$UPPER_LIMIT" ]
		then
			echo $HEAT_OFF > $CTRL_FILE
			echo "#Disabling heater" >> $LOG_FILE
		fi
		if [ "$CURR_TEMP" -lt "$LOWER_LIMIT" ]
		then
			echo $HEAT_ON > $CTRL_FILE
			echo "#Enabling heater" >> $LOG_FILE
		fi
	else
		echo "#Heating state forced, skipping control" >> $LOG_FILE
	fi
	sleep $INTERVAL
	CYCLE=$((CYCLE+1))
done
echo "YaY - Done" >> $LOG_FILE

