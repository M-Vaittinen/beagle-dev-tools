#!/bin/bash

# Use script to read ADC value (script does required filtering) and
# call binary which converts the ADC value to resistance.
#
# First and only parameter for this should be output file where
# resistance is written
# give $1 as parameter for get_resistance
#

ADC_FILE=/savustin/bin/read_meat_temp.sh
RAW_VOLT=`$ADC_FILE`

/savustin/bin/get_resistance $RAW_VOLT $1
