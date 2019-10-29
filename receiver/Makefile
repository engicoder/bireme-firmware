PROJECT_NAME     := receiver
TARGETS          := bireme_receiver
OUTPUT_DIRECTORY := _build

SDK_ROOT := $(NRF_SDK_RCVR)
PROJ_DIR := .

NRFJPROG=$(NRF_TOOLS)/nrfjprog -s $(JLINK_SERIAL)

JAVA=java

$(OUTPUT_DIRECTORY)/bireme_receiver.out: \
  LINKER_SCRIPT  := armgcc/bireme_receiver_gcc_nrf51.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/libraries/uart/app_uart_fifo.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/drivers_nrf/rtc/nrf_drv_rtc.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_ROOT)/components/toolchain/system_nrf51.c \
  $(PROJ_DIR)/main.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR) \
  $(PROJ_DIR)/config\
  $(SDK_ROOT)/components/proprietary_rf/gzll \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/drivers_nrf/rtc \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/fifo \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/timer \

# Libraries common to all targets
LIB_FILES += \
  $(SDK_ROOT)/components/proprietary_rf/gzll/gcc/gzll_nrf51_sd_resources_gcc.a \

# C flags common to all targets
CFLAGS += -DGAZELL_PRESENT
CFLAGS += -DNRF51
CFLAGS += -DUSE_SD_HW_RESOURCES
CFLAGS += -DGAZELL_ALTERNATIVE_RESOURCES
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF51822
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -O3 -g3 -ggdb
CFLAGS += -mfloat-abi=soft
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DGAZELL_PRESENT
ASMFLAGS += -DNRF51
ASMFLAGS += -DUSE_SD_HW_RESOURCES
ASMFLAGS += -DGAZELL_ALTERNATIVE_RESOURCES
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF51822

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections -g3 -ggdb
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: default all clean help flash 

# Default target - first one defined
default: bireme_receiver

help:
	@echo following targets are available:
	@echo 	bireme_receiver

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/bireme_receiver.hex
	@echo Flashing: $<
	$(NRFJPROG) -f nrf51 --program $< --sectorerase
	$(NRFJPROG) -f nrf51 --reset

erase: 
	$(NRFJPROG) -f nrf51 --eraseall

SDK_CONFIG_FILE := $(PROJ_DIR)/config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	$(JAVA) -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
