PROJECT_NAME     := bireme_keyboard
TARGETS          := bireme_keyboard_left bireme_keyboard_right
OUTPUT_DIRECTORY := _build

SDK_ROOT := $(NRF_SDK_KBD)
PROJ_DIR := .

NRFJPROG=$(NRF_TOOLS)/nrfjprog -s $(JLINK_SERIAL)

$(OUTPUT_DIRECTORY)/bireme_keyboard_left.out: \
  LINKER_SCRIPT  := armgcc/bireme_keyboard_gcc_nrf52.ld


$(OUTPUT_DIRECTORY)/bireme_keyboard_right.out: \
  LINKER_SCRIPT  := armgcc/bireme_keyboard_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rtc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/bat_level.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
#  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c 

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components/proprietary_rf/gzll \
  $(SDK_ROOT)/integration/nrfx \
  $(PROJ_DIR) \
  $(PROJ_DIR)/.. \
  $(PROJ_DIR)/config \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/timer \

# Libraries common to all targets
LIB_FILES += \
  $(SDK_ROOT)/components/proprietary_rf/gzll/gcc/gzll_nrf52840_sd_resources_gcc.a \

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DGAZELL_ALTERNATIVE_RESOURCES
CFLAGS += -DGAZELL_PRESENT
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DUSE_SD_HW_RESOURCES
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS 
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DGAZELL_ALTERNATIVE_RESOURCES
ASMFLAGS += -DGAZELL_PRESENT
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DUSE_SD_HW_RESOURCES
ASMFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

bireme_keyboard_left bireme_keyboard_right: CFLAGS += -D__HEAP_SIZE=8192
bireme_keyboard_left bireme_keyboard_right: CFLAGS += -D__STACK_SIZE=8192
bireme_keyboard_left bireme_keyboard_right: ASMFLAGS += -D__HEAP_SIZE=8192
bireme_keyboard_left bireme_keyboard_right: ASMFLAGS += -D__STACK_SIZE=8192

bireme_keyboard_left: CFLAGS += -DLEFT_HAND
bireme_keyboard_right: CFLAGS += -DRIGHT_HAND

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: bireme_keyboard_left

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		bireme_keyboard_left
	@echo		bireme_keyboard_right
	@echo		flash_left            - flashing left side binary
	@echo		flash_right           - flashing right side binary
	@echo   erase_left            - erase left side
	@echo   erase_right           - erase left side

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash_left erase

# Flash the program
flash_left: $(OUTPUT_DIRECTORY)/bireme_keyboard_left.hex
	@echo Flashing: $<
	$(NRFJPROG) -f nrf52 --program $< --sectorerase
	$(NRFJPROG) -f nrf52 --reset

flash_right: $(OUTPUT_DIRECTORY)/bireme_keyboard_right.hex
	@echo Flashing: $<
	$(NRFJPROG) -f nrf52 --program $< --sectorerase
	$(NRFJPROG) -f nrf52 --reset

erase: 
	$(NRFJPROG) -f nrf52 --eraseall
