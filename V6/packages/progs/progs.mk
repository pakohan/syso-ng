################################################################################
#
# progs
#
################################################################################

LED_VERSION = 1.0
LED_SITE = http://www.google.com
LED_LICENSE = GPLv2
LED_LICENSE_FILES = LICENSE

define LED_INSTALL_TARGET_CMDS
	mkdir
	$(INSTALL) -m 0755 -D $(@D)/led5 $(TARGET_DIR)/root/led5
	$(INSTALL) -m 0755 -D $(@D)/switch $(TARGET_DIR)/root/switch
	$(INSTALL) -m 0755 -D $(@D)/led.sh $(TARGET_DIR)/root/led.sh
endef

$(eval $(generic-package))
