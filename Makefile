BBox-archive=busybox-1.21.1.tar.bz2
BBox-folder=busybox-1.21.1

LV=3.4.70

linux-archive=linux-${LV}.tar.xz
linux-folder=linux-${LV}

BRoot-archive=buildroot-2013.08.1.tar.bz2
BRoot-folder=buildroot-2013.08.1

.PHONY: all clean clean-dl

all: ${BBox-folder} ${linux-folder} ${BRoot-folder}

${BBox-archive}:
	wget http://busybox.net/downloads/busybox-1.21.1.tar.bz2

${BBox-folder}: ${BBox-archive}
	tar xfj $<

${linux-archive}:
	wget https://www.kernel.org/pub/linux/kernel/v3.x/${linux-archive}

${linux-folder}: ${linux-archive}
	tar xJf $<

${BRoot-archive}:
	wget http://buildroot.uclibc.org/downloads/buildroot-2013.08.1.tar.bz2

${BRoot-folder}: ${BRoot-archive}
	tar xjf $<

clean:
	rm -rf ${BBox-folder} ${linux-folder} ${BRoot-folder}

clean-dl: clean
	rm ${BBox-archive} ${linux-archive} ${BRoot-archive}
