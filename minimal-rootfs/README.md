#Debian based RootFS for BBB

tar file is splitted due to githubs file size limit. Concatenate using

cat debian-9.3-minimal-armhf-2017-12-09.tar.xz.part_a* > debian-9.3-minimal-armhf-2017-12-09.tar.xz

Then extract the archive to your nfs share. Note that tar contains folder debian-9.3-minimal-armhf-2017-12-09 - and you must either share this folder via nfs - or copy rootfs from this folder to root of your NFS share bbb sees. (Please see uEnv.txt from uboot to see the NFS share path).
