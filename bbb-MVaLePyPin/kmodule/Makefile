obj-m += bbb_pinctrl_drv.o

# Use: make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
# On the BeagleBone Black, kernel headers must be installed:
#   sudo apt-get install linux-headers-$(uname -r)

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

.PHONY: all clean install
