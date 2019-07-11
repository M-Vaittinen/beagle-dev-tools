#!/bin/bash

cd ../../bb-compiler/
source setcc
cd -

if [ "$1" == "clean" ] || [ "$1" == "debug_cfg" ] || [ "$1" == "install" ] || [ "$1" == "dtb" ];
then
	make "$1"
else
	make
fi;
