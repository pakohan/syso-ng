#!/bin/sh


sh output/build/linux-3.10.7/scripts/gen_initramfs_list.sh /home/pakohan/Desktop/syso-ng/buildroot-2013.08.1/output/target >> ../buildroot-2013.08.1/output/build/linux-3.10.7/initramfsconfig > ../buildroot-2013.08.1/output/build/linux-3.10.7/initramfsconfig

echo "nod /dev/console 644 0 0 c 5 1" >> ../buildroot-2013.08.1/output/build/linux-3.10.7/initramfsconfig
echo "slink /init ../bin/busybox 777 1000 1000" >> ../buildroot-2013.08.1/output/build/linux-3.10.7/initramfsconfig

