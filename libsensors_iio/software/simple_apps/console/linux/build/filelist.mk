#### filelist.mk for console_test ####

# helper headers
HEADERS := $(COMMON_DIR)/external_hardware.h
HEADERS += $(COMMON_DIR)/fopenCMake.h
HEADERS += $(COMMON_DIR)/helper.h
HEADERS += $(COMMON_DIR)/mlerrorcode.h
HEADERS += $(COMMON_DIR)/mlsetup.h
HEADERS += $(COMMON_DIR)/slave.h

HEADERS += $(HAL_DIR)/include/mlos.h
HEADERS += $(HAL_DIR)/include/inv_sysfs_utils.h

# sources
SOURCES := $(APP_DIR)/console_test.c

# helper sources
SOURCES += $(HAL_DIR)/linux/inv_sysfs_utils.c
SOURCES += $(HAL_DIR)/linux/mlos_linux.c

INV_SOURCES += $(SOURCES)

VPATH += $(APP_DIR) $(COMMON_DIR) $(HAL_DIR)/linux
