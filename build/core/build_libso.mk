LOCAL_MODULE_PATH := $(OUT_LIB)/$(LOCAL_MODULE).so

$(eval $(call module_common_action,shared))

$(LOCAL_MODULE_PATH): $(LOCAL_COBJ)
	@echo "[LD]    $@ <= $^"
	$(Q)$(CC) -shared -o $@ $^
	$(call strip_files,$@)