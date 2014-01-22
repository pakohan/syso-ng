################################################################################
#
# progs
#
################################################################################

PROGS_VERSION = 1.0
PROGS_SITE = http://www.google.com
PROGS_LICENSE = GPLv2
PROGS_LICENSE_FILES = LICENSE

define PROGS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/led5.exe $(TARGET_DIR)/root/led5.exe
	$(INSTALL) -m 0755 -D $(@D)/switch.exe $(TARGET_DIR)/root/switch.exe
	$(INSTALL) -m 0755 -D $(@D)/led.sh $(TARGET_DIR)/root/led.sh
	$(INSTALL) -m 0755 -D $(@D)/led250.exe $(TARGET_DIR)/root/led250.exe
	$(INSTALL) -m 0755 -D $(@D)/ledmax.exe $(TARGET_DIR)/root/ledmax.exe
	$(INSTALL) -m 0755 -D $(@D)/ledflip.exe $(TARGET_DIR)/root/ledflip.exe
	cp $(@D)/mygpio.ko $(TARGET_DIR)/root/mygpio.ko
endef

$(eval $(generic-package))
