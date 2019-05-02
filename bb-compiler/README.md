#Here's a cross compiler for ARM. You can use it to compile kernel (or programs) for BeagleBone.

__Execute:__

```
source setcc
${CC}gcc --version
```
and if everything is Ok you should see ARM version of gcc used.

Go to linux source folder and execute

```
make ARCH=arm CROSS_COMPILE=${CC} LOADADDR=0x80008000
sudo make ARCH=arm CROSS_COMPILE=${CC} INSTALL_MOD_PATH=PATH_TO_NFS_SHARE modules_install
```



