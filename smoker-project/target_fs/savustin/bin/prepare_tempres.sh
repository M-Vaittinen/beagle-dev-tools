#!/bin/bash



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
