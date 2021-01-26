#!/bin/bash
#
# Helper script called by php script which collects the food temperature probe
# calibration values. User should put the food probe in three known
# temperatures (like use working food probe and a pot with cold water,
# warm water and hot water) and initiate food probe resistance measurements
# in these known temperatures.
#
# PHP script collects the temperatures from user and initiates resistance
# measurements - and passes the values to this script.
#
# This script formats calib_vals.txt file with format that the calc_coeff
# program expects - and runs the program. calc_coeff then computes the
# coefficient values for Steinhart-Hart equation (which we use to convert
# measured NTC food-probe resistances to temperatures when we measure food
# temperature).

TEMP0=$1
TEMP1=$2
TEMP2=$3
RES0=`cat $4`
RES1=`cat $5`
RES2=`cat $6`

cp /savustin/configs/calib_vals.txt /savustin/configs/calib_vals_old.txt
cp /savustin/configs/coeff.txt /savustin/configs/coeff_old.txt

echo "$RES0:$TEMP0" > /savustin/configs/calib_vals.txt
echo "$RES1:$TEMP1" >> /savustin/configs/calib_vals.txt
echo "$RES2:$TEMP2" >> /savustin/configs/calib_vals.txt

/savustin/bin/calc_coeff /savustin/configs/calib_vals.txt /savustin/configs/coeff.txt
