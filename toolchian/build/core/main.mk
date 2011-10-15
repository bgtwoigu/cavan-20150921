ROOT_PATH = $(shell pwd)
BUILD_PATH = $(ROOT_PATH)/build
PACKAGE_PATH = $(ROOT_PATH)/package
PATCH_PATH = $(ROOT_PATH)/patch
SCRIPT_PATH = $(BUILD_PATH)/script
DOWNLOAD_PATH = $(ROOT_PATH)/download

WORK_PATH = $(HOME)/work
SRC_PATH = $(WORK_PATH)/src
UTILS_PATH = $(WORK_PATH)/utils
DECOMP_PATH = $(WORK_PATH)/decomp
MARK_PATH = $(WORK_PATH)/mark
OUT_PATH = $(WORK_PATH)/out

TOOLCHIAN_TT_NAME = $(CAVAN_TARGET_ARCH)-$(CAVAN_TARGET_ARCH)
TOOLCHIAN_BT_NAME = $(CAVAN_BUILD_ARCH)-$(CAVAN_TARGET_ARCH)

TOOLCHIAN_PREFIX = $(WORK_PATH)/$(CAVAN_TARGET_PLAT)
TOOLCHIAN_BT_PATH = $(TOOLCHIAN_PREFIX)/$(TOOLCHIAN_BT_NAME)
TOOLCHIAN_TT_PATH = $(TOOLCHIAN_PREFIX)/$(TOOLCHIAN_TT_NAME)

SYSROOT_BT_PATH = $(TOOLCHIAN_BT_PATH)/sysroot
SYSROOT_TT_PATH = $(TOOLCHIAN_TT_PATH)/sysroot

ROOTFS_PATH = $(WORK_PATH)/rootfs/$(CAVAN_TARGET_ARCH)

OUT_TOOLCHIAN_TT = $(OUT_PATH)/toolchian/$(TOOLCHIAN_TT_NAME)
OUT_TOOLCHIAN_BT = $(OUT_PATH)/toolchian/$(TOOLCHIAN_BT_NAME)
OUT_ROOTFS = $(OUT_PATH)/rootfs/$(CAVAN_TARGET_ARCH)
OUT_UTILS = $(OUT_PATH)/utils

MARK_TOOLCHIAN_TT = $(MARK_PATH)/toolchian/$(TOOLCHIAN_TT_NAME)
MARK_TOOLCHIAN_BT = $(MARK_PATH)/toolchian/$(TOOLCHIAN_BT_NAME)
MARK_ROOTFS = $(MARK_PATH)/rootfs/$(CAVAN_TARGET_ARCH)
MARK_UTILS = $(MARK_PATH)/utils

BUILD_CORE = $(BUILD_PATH)/core
BUILD_TOOLCHIAN = $(BUILD_PATH)/toolchian
BUILD_ROOTFS = $(BUILD_PATH)/rootfs
BUILD_UTILS = $(BUILD_PATH)/utils

MARK_TOOLCHIAN_TT_READY = $(MARK_TOOLCHIAN_TT)/ready
MARK_TOOLCHIAN_BT_READY = $(MARK_TOOLCHIAN_BT)/ready
MARK_ROOTFS_READY = $(MARK_ROOTFS)/ready
MARK_UTILS_READY = $(MARK_UTILS)/ready

MAKEFILE_TOOLCHIAN = $(BUILD_TOOLCHIAN)/main.mk
MAKEFILE_ROOTFS = $(BUILD_ROOTFS)/main.mk
MAKEFILE_UTILS = $(BUILD_UTILS)/main.mk
MAKEFILE_DEFINES = $(BUILD_CORE)/defines.mk

PYTHON_PARSER = $(SCRIPT_PATH)/parser.py
BASH_DOWNLOAD = ${SCRIPT_PATH}/download.sh

MARK_HOST_APPS = $(MARK_UTILS)/environment

define set_path
PATH := $1/usr/local/sbin:$1/usr/local/bin:$1/usr/sbin:$1/usr/bin:$1/sbin:$1/bin:$(PATH)
endef

$(eval $(call set_path,$(UTILS_PATH)))
$(eval $(call set_path,$(TOOLCHIAN_BT_PATH)))

DOWNLOAD_COMMAND = bash $(BASH_DOWNLOAD)

export ROOT_PATH PACKAGE_PATH BUILD_PATH PATCH_PATH SCRIPT_PATH
export SRC_PATH ROOTFS_PATH UTILS_PATH DECOMP_PATH PATH DOWNLOAD_PATH
export OUT_PATH OUT_UTILS OUT_ROOTFS OUT_TOOLCHIAN_BT OUT_TOOLCHIAN_TT
export BUILD_CORE BUILD_TOOLCHIAN BUILD_ROOTFS BUILD_UTILS
export MARK_PATH MARK_ROOTFS MARK_UTILS
export MARK_TOOLCHIAN_BT MARK_TOOLCHIAN_TT
export MARK_ROOTFS_READY MARK_UTILS_READY
export MARK_TOOLCHIAN_TT_READY MARK_TOOLCHIAN_BT_READY
export MAKEFILE_DEFINES
export PYTHON_PARSER XML_APPLICATION BASH_DOWNLOAD DOWNLOAD_COMMAND
export TOOLCHIAN_TT_PATH TOOLCHIAN_BT_PATH
export SYSROOT_BT_PATH SYSROOT_TT_PATH

$(info ============================================================)
$(info CAVAN_BUILD_ARCH = $(CAVAN_BUILD_ARCH))
$(info CAVAN_BUILD_PLAT = $(CAVAN_BUILD_PLAT))
$(info CAVAN_TARGET_ARCH = $(CAVAN_TARGET_ARCH))
$(info CAVAN_TARGET_PLAT = $(CAVAN_TARGET_PLAT))
$(info CPU_BINUTILS_OPTION = $(CPU_BINUTILS_OPTION))
$(info CPU_GCC_OPTION = $(CPU_GCC_OPTION))
$(info KERNEL_VERSION = $(KERNEL_VERSION))
$(info BINUTILS_VERSION = $(BINUTILS_VERSION))
$(info GCC_VERSION = $(GCC_VERSION))
$(info GLIBC_VERSION = $(GLIBC_VERSION))
$(info PACKAGE_PATH = $(PACKAGE_PATH))
$(info PATCH_PATH = $(PATCH_PATH))
$(info ============================================================)

include $(MAKEFILE_DEFINES)

toolchian toolchian-bt: $(MARK_TOOLCHIAN_BT_READY)
	$(Q)echo "$@ compile successfull"

toolchian-tt: $(MARK_TOOLCHIAN_TT_READY)
	$(Q)echo "$@ compile successfull"

rootfs: $(MARK_ROOTFS_READY)
	$(Q)echo "$@ compile successfull"

utils: $(MARK_UTILS_READY)
	$(Q)echo "$@ compile successfull"

environment: build_env $(MARK_HOST_APPS)
	$(Q)echo "$@ install successfull"

$(MARK_HOST_APPS):
	$(Q)for pkg in $(HOST_APPS); \
	do \
		echo "$(APT_GET) $${pkg}"; \
		$(APT_GET) $${pkg}; \
	done
	$(call generate_mark)

$(MARK_ROOTFS_READY): $(MARK_TOOLCHIAN_BT_READY)
	$(Q)+make -f $(MAKEFILE_ROOTFS)

$(MARK_TOOLCHIAN_TT_READY): $(MARK_TOOLCHIAN_BT_READY)
	$(Q)+make CAVAN_HOST_ARCH="$(CAVAN_TARGET_ARCH)" -f $(MAKEFILE_TOOLCHIAN)
	$(Q)echo "$(TOOLCHIAN_TT_NAME) toolchian build successfull"

$(MARK_TOOLCHIAN_BT_READY): $(MARK_UTILS_READY)
	$(Q)+make CAVAN_HOST_ARCH="$(CAVAN_BUILD_ARCH)" -f $(MAKEFILE_TOOLCHIAN)
	$(Q)echo "$(TOOLCHIAN_BT_NAME) toolchian build successfull"

$(MARK_UTILS_READY): build_env
	$(Q)+make -f $(MAKEFILE_UTILS)

clean:
	$(Q)rm $(DECOMP_PATH) $(SRC_PATH) $(OUT_PATH) -rfv

distclean:
	$(Q)rm $(WORK_PATH) -rfv

build_env:
	$(Q)if test -L $(WORK_PATH) -o ! -d $(WORK_PATH); \
	then \
		rm $(WORK_PATH) -rfv  && mkdir $(WORK_PATH) -pv; \
	fi
	$(Q)mkdir $(TOOLCHIAN_TT_PATH) $(TOOLCHIAN_BT_PATH) $(OUT_TOOLCHIAN_TT) $(OUT_TOOLCHIAN_BT) -pv
	$(Q)mkdir $(SRC_PATH) $(UTILS_PATH) $(OUT_UTILS) $(OUT_ROOTFS) $(DECOMP_PATH) $(DOWNLOAD_PATH) -pv
	$(Q)mkdir $(MARK_ROOTFS) $(MARK_UTILS) $(MARK_TOOLCHIAN_TT) $(MARK_TOOLCHIAN_BT) -pv

.PHONY: build_env
