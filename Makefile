BBox-archive=busybox-1.21.1.tar.bz2
BBox-folder=busybox-1.21.1

LV=3.4.70

linux-archive=linux-${LV}.tar.xz
linux-folder=linux-${LV}

.PHONY: all clean clean-dl

all: ${BBox-folder} ${linux-folder}

${BBox-archive}:
	wget http://busybox.net/downloads/busybox-1.21.1.tar.bz2

${BBox-folder}: ${BBox-archive}
	tar xfj $<

${linux-archive}:
	wget https://www.kernel.org/pub/linux/kernel/v3.x/${linux-archive}

${linux-folder}: ${linux-archive}
	tar xJf $<

clean:
	rm -rf ${BBox-folder} ${linux-folder}

clean-dl: clean
	rm ${BBox-archive} ${linux-archive}
