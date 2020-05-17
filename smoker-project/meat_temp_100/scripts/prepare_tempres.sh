#!/bin/bash



TEMP0=$1
TEMP1=$2
TEMP2=$3
RES0=`cat $4`
RES1=`cat $5`
RES2=`cat $6`

cp /calib_vals.txt /calib_vals_old.txt
cp /coeff.txt /coeff_old.txt

echo "$RES0:$TEMP0 > /calib_vals.txt"
echo "$RES1:$TEMP1 >> /calib_vals.txt"
echo "$RES2:$TEMP2 >> /calib_vals.txt"

/home/debian/bin/calc_coeff /calib_vals.txt /coeff.txt
