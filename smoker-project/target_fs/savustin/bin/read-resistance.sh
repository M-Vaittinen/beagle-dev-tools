#!/bin/bash

#Read ADC value and call executable which calcs resistance.
#First and only parameter for this should be output file where
#resistance is written
#give $1 as parameter for get_resistance
#

ADC_FILE=/savustin/bin/read_meat_temp.sh
RAW_VOLT=`$ADC_FILE`

/savustin/bin/get_resistance $RAW_VOLT $1
