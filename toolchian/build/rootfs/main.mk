XML_CONFIG = $(BUILD_ROOTFS)/config.xml

SB2_LIBTOOL_VERSION = 1.5.26
SB2_LIBTOOL_NAME = libtool-$(SB2_LIBTOOL_VERSION)
SB2_INIT_MARK = $(MARK_ROOTFS)/sb2_init
SB2_CONFIG_PATH = $(HOME)/.scratchbox2
ROOTFS_BASE = $(BUILD_ROOTFS)/base

include $(MAKEFILE_DEFINES)

all: $(MARK_ROOTFS_READY)
	$(Q)echo "ROOTFS compile successfull"

$(MARK_ROOTFS_READY): $(SB2_INIT_MARK)
	$(call auto_make,install_rootfs,$(MARK_ROOTFS),$(OUT_ROOTFS),$(XML_CONFIG))

$(SB2_INIT_MARK):
	$(Q)rm $(ROOTFS_PATH) -rf && mkdir $(ROOTFS_PATH) -pv && cp $(ROOTFS_BASE)/* $(ROOTFS_PATH) -av
	$(Q)cd $(ROOTFS_PATH) && mkdir bin sbin root home/cavan lib usr/lib libexec tmp proc sys dev etc usr/bin usr/sbin var/run -pv
	$(Q)ln -vsf bash $(ROOTFS_PATH)/bin/sh
	$(Q)ln -vsf vim $(ROOTFS_PATH)/usr/bin/vi
	$(Q)test "$$(id -u)" = "0" && mknod $(ROOTFS_PATH)/dev/console c 5 1 || echo "No permision mknod"
	$(Q)cp $(SYSROOT_BT_PATH)/* $(TOOLCHIAN_BT_PATH)/$(CAVAN_TARGET_PLAT)/lib $(ROOTFS_PATH) -av
	$(Q)cp $(TOOLCHIAN_BT_PATH)/$(CAVAN_TARGET_PLAT)/include $(ROOTFS_PATH)/usr -av
	$(eval SB2_LIBTOOL_PACKAGE = $(firstword $(wildcard $(DOWNLOAD_PATH)/$(SB2_LIBTOOL_NAME).tar.gz $(PACKAGE_PATH)/$(SB2_LIBTOOL_NAME).tar.gz)))
	$(Q)test -n "$(SB2_LIBTOOL_PACKAGE)" && rm $(SB2_CONFIG_PATH) -rf && mkdir $(SB2_CONFIG_PATH) -pv && cp $(SB2_LIBTOOL_PACKAGE) $(SB2_CONFIG_PATH) -av
	$(Q)cd $(ROOTFS_PATH) && sb2-init -c qemu-$(CAVAN_TARGET_ARCH) $(CAVAN_TARGET_PLAT) $(CAVAN_TARGET_PLAT)-gcc
	$(call generate_mark)
