-include variables
-include ../server.host

USERPROG=../${BROOT_VERSION}/output/${KERNEL_VERSION}/usr/initramfs/bin/hello
ROOTFS=../${BROOT_VERSION}/output/${KERNEL_VERSION}/usr/initramfs
LINUX_CONFIG=../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config
BUSYBOX_CONFIG=../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config
BROOT_CONFIG=../${BROOT_VERSION}/.config

.PHONY: all clean start debug ${TARGET}

.NOTPARALLEL:

all: ${TARGET}

start: ${TARGET}
	${QEMU} -kernel ${TARGET}

debug: ${TARGET}
	${QEMU} ${DEBUG} -kernel ${TARGET}

gdb:
	@echo "target extended-remote ip:port"
	/opt/toolchains/Sourcery-CodeBench-ARM-2013.05/bin/arm-none-linux-gnueabi-gdb

buildroot-config:
	cp config-buildroot ../${BROOT_VERSION}/.config
	make -C ../${BROOT_VERSION} nconfig
	cp ../${BROOT_VERSION}/.config config-buildroot

linux-config:
	cp config-linux ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config
	make -C ../${BROOT_VERSION} linux-nconfig
	cp ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config ../${BROOT_VERSION}/board/qemu/arm-versatile/${KERNEL_VERSION}.config
	cp ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config config-linux

busybox-config:
	cp config-busybox ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config
	make -C ../${BROOT_VERSION} busybox-menuconfig
	cp ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config ../${BROOT_VERSION}/package/busybox/${BBOX_VERSION}.config
	cp ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config config-busybox


${TARGET}: ${LINUX_CONFIG} ${BUSYBOX_CONFIG} ${BROOT_CONFIG}
	rm -f ../${BROOT_VERSION}/output/target/usr/bin/userprog
	rm -rf ../${BROOT_VERSION}/output/build/userprog-1.0/
	rm -f ../${BROOT_VERSION}/output/images/zImage
	make -C ../${BROOT_VERSION} -j ${JOBS}

${BUSYBOX_CONFIG}: config-busybox
	cp config-busybox ../${BROOT_VERSION}/package/busybox/${BBOX_VERSION}.config
	cp config-busybox ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/.config

${LINUX_CONFIG}: config-linux
	cp config-linux ../${BROOT_VERSION}/board/qemu/arm-versatile/${KERNEL_VERSION}.config
	cp config-linux ../${BROOT_VERSION}/output/build/${KERNEL_VERSION}/.config

${BROOT_CONFIG}: config-buildroot
	cp config-buildroot ../${BROOT_VERSION}/.config

clean:
	rm -rf ../${BROOT_VERSION}
	tar xjf ../${BROOT_VERSION}.tar.bz2 -C ../
	cp -R dl ../${BROOT_VERSION}/
	mkdir -p ../${BROOT_VERSION}/output/build/linux-3.10.7/
	mkdir -p ../${BROOT_VERSION}/output/build/busybox-1.21.1/
	cp Config.in ../${BROOT_VERSION}/package/
	cp -R userprog ../${BROOT_VERSION}/package/
