#Example driver

Here we have a kernel module and build.sh && Makefile for compiling the module
You need kernel source which corresponds to kernel in your BBB. You should see linux_kernel_compilation folder for documentation on how to build the kernel. Additionally you need the cross compiler (see bb-compiler).

1. Edit build.sh so that kernel source directory matches your setup.
2. type Make
3. transfer test.ko kernelmodule to BBB and insmod it.
4. transfer test.dtbo device-tree binary to BBB and merge it to running system*
5. see dmesg for prints from test.dtbo

__regarding step 4 - We currently have no in-tree userspace access to device-tree overlay mechanisms. There are few custom ones (seems like one with configfs is actually pretty widely supported) and you may try your luck with them. I also wrote one (see overlay_merger). I tested one I wrote works with kernel 4.16.2 - but as the overlay area seems to be work-in-progress - I am not promising anything more.__
