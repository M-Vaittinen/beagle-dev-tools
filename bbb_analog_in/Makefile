
all: dtb
dtb: bbb_analog_inputs.dts
	dtc -O dtb -o bbb_analog_inputs.dtbo -b 0 -@ bbb_analog_inputs.dts

clean:
	rm -rf bbb_analog_inputs.dtbo
