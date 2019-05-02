#!/bin/bash

#export KERNEL_DIR=/home/mvaittin/MFD/mfd
#export KERNEL_DIR=/home/mvaittin/REGULATOR/regulator
export KERNEL_DIR=/home/mvaittin/linux-next/linux-next

cd ../bb-compiler/
source setcc
cd -

if [ "$1" == "clean" ] || [ "$1" == "debug_cfg" ];
then
	make "$1"
else
	make
fi;
