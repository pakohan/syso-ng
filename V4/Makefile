-include variables
-include ../server.host

.PHONY: all clean start debug ${TARGET} module

.NOTPARALLEL:

all: ${TARGET}

start: ${TARGET}
	${QEMU} -kernel ${TARGET}

debug: ${TARGET}
	${QEMU} ${DEBUG} -kernel ${TARGET}

gdb:
	@echo "target extended-remote ip:port"
	${CROSS_CC}gdb

module:
	CROSS_COMPILE=${CROSS_CC} make -C ${LINUX_DIR} M=$(PWD)/module ARCH=arm

copy-module:
	scp module/*.ko root@192.168.29.166:/lib/modules/${KERNEL_VERSION}/kernel/drivers

buildroot-config:
	cp config-buildroot ../${BROOT_VERSION}/.config
	make -C ../${BROOT_VERSION} nconfig
	cp ../${BROOT_VERSION}/.config config-buildroot

linux-config:
	cp config-linux ../${BROOT_VERSION}/output/build/linux-${KERNEL_VERSION}/.config
	make -C ../${BROOT_VERSION} linux-nconfig
	cp ../${BROOT_VERSION}/output/build/linux-${KERNEL_VERSION}/.config ../${BROOT_VERSION}/board/qemu/arm-versatile/linux-${KERNEL_VERSION}.config
	cp ../${BROOT_VERSION}/output/build/linux-${KERNEL_VERSION}/.config config-linux

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
	cp config-linux ../${BROOT_VERSION}/board/qemu/arm-versatile/linux-${KERNEL_VERSION}.config
	cp config-linux ../${BROOT_VERSION}/output/build/linux-${KERNEL_VERSION}/.config

${BROOT_CONFIG}: config-buildroot
	cp config-buildroot ../${BROOT_VERSION}/.config

clean:
	rm -rf ../${BROOT_VERSION}
	tar xjf ../${BROOT_VERSION}.tar.bz2 -C ../
	cp -R dl ../${BROOT_VERSION}/
	mkdir -p ../${BROOT_VERSION}/output/build/linux-${KERNEL_VERSION}/
	mkdir -p ../${BROOT_VERSION}/output/build/${BBOX_VERSION}/
	cp Config.in ../${BROOT_VERSION}/package/
	cp -R userprog ../${BROOT_VERSION}/package/

clean-modules:
	make -C module clean
