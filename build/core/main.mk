ifeq ("$(ARCH)","")
ARCH = x86
CROSS_COMPILE =
else
ifeq ("$(ARCH)","arm")
ifeq ("$(CROSS_COMPILE)","")
CROSS_COMPILE = arm-linux-
endif
endif
endif

ifeq ("$(origin BT)","command line")
BUILD_TYPE = $(BT)
else
ifeq ("$(BUILD_TYPE)","")
BUILD_TYPE = release
endif
endif

OUT_DIR = out
LIB_DIR = lib
APP_DIR = app
INCLUDE_DIR = include
BUILD_DIR = build
BUILD_CORE = $(BUILD_DIR)/core

TARGET_DIR = $(OUT_DIR)/$(ARCH)
TARGET_LIB = $(TARGET_DIR)/lib
TARGET_ELF = $(TARGET_DIR)/bin
TARGET_APP = $(TARGET_DIR)/app

DEPEND_NAME = depend.mk
LIB_DEPEND = $(TARGET_LIB)/$(DEPEND_NAME)
APP_DEPEND = $(TARGET_APP)/$(DEPEND_NAME)
ELF_DEPEND = $(TARGET_ELF)/$(DEPEND_NAME)

CAVAN_NAME = cavan
TARGET_OBJ = $(TARGET_LIB)/lib$(CAVAN_NAME).o
TARGET_LIBA = $(TARGET_LIB)/lib$(CAVAN_NAME).a
TARGET_LIBSO = $(TARGET_LIB)/lib$(CAVAN_NAME).so

APPS_MAKEFILE = $(BUILD_CORE)/application.mk
LIBS_MAKEFILE = $(BUILD_CORE)/library.mk
DEFINES_MAKEFILE = $(BUILD_CORE)/defines.mk

CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar

CFLAGS +=	-Wall -Wundef -Werror -Wstrict-prototypes -Wno-trigraphs \
			-Werror-implicit-function-declaration -Wno-format-security \
			-fno-strict-aliasing -g -O2 -I$(INCLUDE_DIR) -I. -DARCH=$(ARCH)
ASFLAGS +=	$(CFLAGS) -D__ASM__

ifeq ("$(findstring release,$(BUILD_TYPE))","")
ifeq ("$(findstring static,$(BUILD_TYPE))","")
LOCAL_LDFLAGS += -Wl,-rpath,$(TARGET_LIB)
else
LOCAL_LDFLAGS += -static
endif
LOCAL_LDFLAGS += -L$(TARGET_LIB) -l$(CAVAN_NAME)
else
LDFLAGS += $(TARGET_OBJ)
endif
LDFLAGS := -s $(LOCAL_LDFLAGS) $(LDFLAGS)

LIB_SOURCE = $(wildcard lib/*.c)
APP_SOURCE  = $(wildcard app/*.c)

ifeq ("$(origin APP)","command line")
ifeq ("$(wildcard $(APP)/cavan.mk)","$(APP)/cavan.mk")
include $(APP)/cavan.mk
APP_SOURCE += $(addprefix $(APP)/,$(source-app))
LIB_SOURCE += $(addprefix $(APP)/,$(source-lib))
else
APP_SOURCE += $(wildcard $(APP)/*.c)
endif
endif

ifeq ("$(origin LIB)","command line")
ifeq ("$(wildcard $(LIB)/cavan.mk)","$(LIB)/cavan.mk")
include $(LIB)/cavan.mk
APP_SOURCE += $(addprefix $(LIB)/,$(source-app))
LIB_SOURCE += $(addprefix $(LIB)/,$(source-lib))
else
LIB_SOURCE += $(wildcard $(LIB)/*.c)
endif
endif

LIB_SOURCE += $(wildcard lib/*.c)
LIB_OBJECT = $(patsubst %.c,$(TARGET_LIB)/%.o,$(notdir $(LIB_SOURCE)))
APP_SOURCE  += $(wildcard app/*.c)
APP_OBJECT = $(patsubst %.c,$(TARGET_APP)/%.o,$(notdir $(APP_SOURCE)))
ifeq ("$(strip $(ELF_PREFIX))","")
ELF_OBJECT = $(patsubst %.c,$(TARGET_ELF)/%,$(notdir $(APP_SOURCE)))
else
ELF_OBJECT = $(patsubst %.c,$(TARGET_ELF)/$(ELF_PREFIX)-%,$(notdir $(APP_SOURCE)))
endif

ifeq ("$(Q)","@")
MAKEFLAGS += --no-print-directory
endif

$(info ============================================================)
$(info ARCH = $(ARCH))
$(info CROSS_COMPILE = $(CROSS_COMPILE))
$(info BUILD_TYPE = $(BUILD_TYPE))
$(info ============================================================)

export CC LD AR CFLAGS LDFLAGS
export OUT_DIR LIB_DIR APP_DIR INCLUDE_DIR BUILD_DIR BUILD_CORE
export TARGET_DIR TARGET_LIB TARGET_ELF TARGET_APP
export LIB_DEPEND APP_DEPEND ELF_DEPEND
export TARGET_OBJ TARGET_LIBA TARGET_LIBSO
export LIB_SOURCE LIB_OBJECT APP_SOURCE APP_OBJECT ELF_OBJECT
export APPS_MAKEFILE LIBS_MAKEFILE DEFINES_MAKEFILE

all: app

app: lib $(TARGET_APP) $(TARGET_ELF) $(APP_DEPEND) $(ELF_DEPEND)
	$(Q)make -f $(APPS_MAKEFILE)

lib: $(TARGET_LIB) $(LIB_DEPEND)
	$(Q)make -f $(LIBS_MAKEFILE)

$(LIB_DEPEND): $(LIB_SOURCE)
	$(call build_obj_depend)

$(APP_DEPEND): $(APP_SOURCE)
	$(call build_obj_depend)

$(ELF_DEPEND): $(APP_SOURCE)
	$(call build_elf_depend,$(TARGET_APP),$(ELF_PREFIX))

$(TARGET_LIB) $(TARGET_ELF) $(TARGET_APP): $(TARGET_DIR)
	$(Q)mkdir $@ -pv

$(TARGET_DIR): $(OUT_DIR)
	$(Q)mkdir $@ -pv

$(OUT_DIR):
	$(Q)mkdir $@ -pv

clean:
	$(Q)rm $(TARGET_ELF) -rf

distclean:
	$(Q)rm $(TARGET_DIR) -rf

.PHONY: lib app $(APP_DEPEND) $(LIB_DEPEND) $(ELF_DEPEND)

include $(DEFINES_MAKEFILE)
