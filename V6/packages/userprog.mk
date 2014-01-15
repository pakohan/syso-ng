################################################################################
#
# userprog 
#
################################################################################

USERPROG_VERSION = 1.0
USERPROG_SITE = http://www.google.com
USERPROG_LICENSE = GPLv2
USERPROG_LICENSE_FILES = LICENsE

ifeq ($(BR2_PACKAGE_USERPROG_STATIC),y)
	export USERPROG_STATIC=-static
else
	export USERPROG_STATIC=
endif

define USERPROG_BUILD_CMDS
	cp -f package/userprog/Makefile $(@D)
	$(TARGET_CONFIGURE_OPTS) make -C $(@D)
endef

define USERPROG_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/userprog $(TARGET_DIR)/usr/bin/userprog
endef

$(eval $(generic-package))
