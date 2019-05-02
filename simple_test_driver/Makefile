obj-m += test.o


all: dtb
	make -C ${KERNEL_DIR} CROSS_COMPILE=${CC} ARCH=arm M=$(PWD) modules
dtb: bbb_i2c_overlay.dts
	dtc -O dtb -o bbb_i2c_overlay-00A0.dtbo -b 0 -@ bbb_i2c_overlay.dts

clean:
	make -C ${KERNEL_DIR} CROSS_COMPILE=${CC} ARCH=arm M=$(PWD) clean
