-include variables
-include ../server.host

USERPROG=../${BROOT_VERSION}/output/${KERNEL_VERSION}/usr/initramfs/bin/hello
ROOTFS=../${BROOT_VERSION}/output/${KERNEL_VERSION}/usr/initramfs
LINUX_CONFIG=../${BROOT_VERSION}/output/${KERNEL_VERSION}/.config

.PHONY: all clean clean-rootfs config-linux config-busybox start config-buildroot

.NOTPARALLEL:

all: ${TARGET}

start: ${TARGET}
	${QEMU} -kernel ${TARGET} -qmp tcp:localhost:25565,server,nowait


config-buildroot:
	cp config-buildroot ../${BROOT_VERSION}/.config
	make -C ../${BROOT_VERSION} menuconfig
	cp ../${BROOT_VERSION}/.config config-buildroot

config-linux:
	cp config-linux ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config
	make -C ../${BROOT_VERSION} linux-nconfig
	cp ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config ../${BROOT_VERSION}/board/qemu/arm-versatile/${KERNEL_VERSION}.config
	cp ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config config-linux

config-busybox:
	cp config-busybox ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config
	make -C ../${BROOT_VERSION} busybox-menuconfig
	cp ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config ../${BROOT_VERSION}/package/busybox/${BBOX_VERSION}.config
	cp ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config config-busybox


${TARGET}: ${ROOTFS} ${LINUX_CONFIG} ${BUSYBOX_CONFIG}
	#sh ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/scripts/gen_initramfs_list.sh ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/usr/initramfs/ > ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/initramfsconfig
	#cat initramfsconfig_nodes >> ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/initramfsconfig
	make -C ../${BROOT_VERSION}




${ROOTFS}:
	#cp -R initramfs ../${BROOT_VERSION}/output/${KERNEL_VERSION}/usr

${USERPROG}: ${ROOTFS} init.c
	mkdir -p ${ROOTFS}/bin/
	${CROSS_COMPILE}gcc -s -static ${EXTRA_CFLAGS} init.c -o $@

${BUSYBOX_CONFIG}:
	cp config-busybox /${BROOT_VERSION}/package/busybox/${BBOX_VERSION}.config
	cp config-busybox /${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config

${LINUX_CONFIG}:
	cp config-linux ../${BROOT_VERSION}/board/qemu/arm-versatile/${KERNEL_VERSION}.config
	cp config-linux ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config

clean: clean-rootfs

clean-rootfs:
	
