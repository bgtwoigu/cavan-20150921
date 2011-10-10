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

SYSROOT_PATH = $(WORK_PATH)/sysroot/$(CAVAN_TARGET_ARCH)
ROOTFS_PATH = $(WORK_PATH)/rootfs/$(CAVAN_TARGET_ARCH)
OUT_TOOLCHIAN = $(OUT_PATH)/toolchian/$(CAVAN_TARGET_ARCH)
OUT_ROOTFS = $(OUT_PATH)/rootfs/$(CAVAN_TARGET_ARCH)
OUT_UTILS = $(OUT_PATH)/utils
MARK_TOOLCHIAN = $(MARK_PATH)/toolchian/$(CAVAN_TARGET_ARCH)
MARK_ROOTFS = $(MARK_PATH)/rootfs/$(CAVAN_TARGET_ARCH)
MARK_UTILS = $(MARK_PATH)/utils

BUILD_CORE = $(BUILD_PATH)/core
BUILD_TOOLCHIAN = $(BUILD_PATH)/toolchian
BUILD_ROOTFS = $(BUILD_PATH)/rootfs
BUILD_UTILS = $(BUILD_PATH)/utils

MARK_TOOLCHIAN_READY = $(MARK_TOOLCHIAN)/ready
MARK_ROOTFS_READY = $(MARK_ROOTFS)/ready
MARK_UTILS_READY = $(MARK_UTILS)/ready

MAKEFILE_TOOLCHIAN = $(BUILD_TOOLCHIAN)/main.mk
MAKEFILE_ROOTFS = $(BUILD_ROOTFS)/main.mk
MAKEFILE_UTILS = $(BUILD_UTILS)/main.mk
MAKEFILE_DEFINES = $(BUILD_CORE)/defines.mk
MAKEFILE_INSTALL = $(BUILD_CORE)/install.mk

PYTHON_PARSER = $(SCRIPT_PATH)/parser.py
BASH_DOWNLOAD = ${SCRIPT_PATH}/download.sh

MARK_HOST_APPS = $(MARK_UTILS)/environment

SYSROOT_APPS = $(SYSROOT_PATH)/bin:$(SYSROOT_PATH)/bin:$(SYSROOT_PATH)/usr/bin:$(SYSROOT_PATH)/usr/sbin:$(SYSROOT_PATH)/usr/local/bin:$(SYSROOT_PATH)/usr/local/sbin
UTILS_APPS = $(UTILS_PATH)/bin:$(UTILS_PATH)/bin:$(UTILS_PATH)/usr/bin:$(UTILS_PATH)/usr/sbin:$(UTILS_PATH)/usr/local/bin:$(UTILS_PATH)/usr/local/sbin
PATH := $(SYSROOT_APPS):$(UTILS_APPS):$(PATH)

DOWNLOAD_COMMAND = bash $(BASH_DOWNLOAD)

export ROOT_PATH PACKAGE_PATH BUILD_PATH PATCH_PATH SCRIPT_PATH
export SRC_PATH SYSROOT_PATH ROOTFS_PATH UTILS_PATH DECOMP_PATH PATH DOWNLOAD_PATH
export OUT_PATH OUT_UTILS OUT_TOOLCHIAN OUT_ROOTFS
export BUILD_CORE BUILD_TOOLCHIAN BUILD_ROOTFS BUILD_UTILS
export MARK_PATH MARK_TOOLCHIAN MARK_ROOTFS MARK_UTILS
export MARK_TOOLCHIAN_READY MARK_ROOTFS_READY MARK_UTILS_READY
export MAKEFILE_DEFINES MAKEFILE_INSTALL
export PYTHON_PARSER XML_APPLICATION BASH_DOWNLOAD DOWNLOAD_COMMAND

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

toolchian: $(MARK_TOOLCHIAN_READY)
	$(Q)echo "Toolchian compile successfull"

rootfs: $(MARK_ROOTFS_READY)
	$(Q)echo "Rootfs compile successfull"

utils: $(MARK_UTILS_READY)
	$(Q)echo "Utils compile successfull"

environment: build_env $(MARK_HOST_APPS)
	$(Q)echo "Host app install successfull"

$(MARK_HOST_APPS):
	$(Q)sudo apt-get install $(HOST_APPS)
	$(call generate_mark)

$(MARK_ROOTFS_READY): $(MARK_TOOLCHIAN_READY)
	$(Q)+make -f $(MAKEFILE_ROOTFS)

$(MARK_TOOLCHIAN_READY): $(MARK_UTILS_READY)
	$(Q)+make -f $(MAKEFILE_TOOLCHIAN)

$(MARK_UTILS_READY): build_env
	$(Q)+make -f $(MAKEFILE_UTILS)

clean:
	$(Q)rm $(DECOMP_PATH) $(SRC_PATH) $(OUT_PATH) -rfv

distclean:
	$(Q)rm $(WORK_PATH) -rfv

build_env:
	$(Q)mkdir $(SRC_PATH) $(SYSROOT_PATH) $(UTILS_PATH) $(OUT_UTILS) $(OUT_TOOLCHIAN) $(OUT_ROOTFS) $(DECOMP_PATH) $(DOWNLOAD_PATH) -pv
	$(Q)mkdir $(MARK_TOOLCHIAN) $(MARK_ROOTFS) $(MARK_UTILS) -pv

.PHONY: build_env
