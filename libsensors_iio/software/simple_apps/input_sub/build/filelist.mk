#### filelist.mk for input_gyro ####

# helper headers
HEADERS := $(MPL_DIR)/authenticate.h
#HEADERS += 

# sources
SOURCES := $(APP_DIR)/test_input_gyro.c

INV_SOURCES += $(SOURCES)

#VPATH += $(APP_DIR) $(COMMON_DIR) $(HAL_DIR)/linux
VPATH += $(APP_DIR)
