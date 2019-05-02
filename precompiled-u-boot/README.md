#Here's a u-boot binary which can be used on beagle bone black if you don't want to compile your own.


+ Copy the binaries and uEnv.txt to your SD card boot partition.
+ Edit the uEnv.txt file so that server IP address, NFS share path etc match to your server settings.


####If you don't have SD card with suitable boot partition, you can create it accirding to instructions below:

###Prepare a boot partition to SD card as explained here:
[www.armhf.com]: [http://www.armhf.com/boards/beaglebone-black/bbb-sd-install/]
or
[github.com/linneman]: [https://github.com/linneman/planck/wiki/How-to-create-a-Boot-SD-Card-for-the-BeagleBone-black]

####(Text below is copied from one of the links above)

> Insert the microSD Card into your computer and observe which device it registers as by typing 
```
ls /dev/sd*.
```
> If you are uncertain, remove the microSD Card and the entry should go away. Once you know which device your microSD Card is, follow the instructions below replacing /dev/sdX with the name of the microSD Card in your system.
> Begin partitioning the microSD Card by typing 
```
fdisk /dev/sdX
```
>    Initialize a new partition table by selecting o, then verify the partition table is empty by selecting p.
>    Create a boot partition by selecting n for 'new', then p for 'primary', and 1 to specify the first partition. Press enter to accept the default first sector and specify 4095 for the last sector.
>    Change the partition type to FAT16 by selecting t for 'type' and e for 'W95 FAT16 (LBA)'.
>    Set the partition bootable by selecting a then 1.
>    Next, create the data partition for the root filesystem by selecting n for 'new', then p for 'primary', and 2 to specify the second partition. Accept the default values for the first and last sectors by pressing enter twice.
>    Press p to 'print' the partition table. It should look similar to the one below.
>
>    Disk /dev/sdb: 7948 MB, 7948206080 bytes
>    255 heads, 63 sectors/track, 966 cylinders, total 15523840 sectors
>    Units = sectors of 1 * 512 = 512 bytes
>    Sector size (logical/physical): 512 bytes / 512 bytes
>    I/O size (minimum/optimal): 512 bytes / 512 bytes
>    Disk identifier: 0xafb3f87b

<pre>

       Device Boot      Start         End      Blocks   Id  System
    /dev/sdb1   *        2048        4095        1024    e  W95 FAT16 (LBA)
    /dev/sdb2            4096    15523839     7759872   83  Linux
</pre>
>    Finally, commit the changes by selecting w to 'write' the partition table and exit fdisk.

> __Format the Partitions__

>    Format partition 1 as FAT by typing
```
mkfs.vfat /dev/sdX1
```
>    Format partition 2 as ext4 by typing
```
mkfs.ext4 /dev/sdX2
```



