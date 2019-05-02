#!/bin/bash

export KERNEL_DIR=/home/mvaittin/MFD/mfd

cd ../bb-compiler/
source setcc
cd -

if [ "$1" == "clean" ];
then
	make clean
else
	make
fi;
