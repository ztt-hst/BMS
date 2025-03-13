################################################################################
# File Directory
################################################################################
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(patsubst %/,%,$(dir $(MKFILE_PATH)))

################################################################################
# Compiler Toolchain
################################################################################
#GCC_PATH := $(CURRENT_DIR)/Toolchain/bin
PREFIX   := arm-none-eabi-

ifdef GCC_PATH
CC := $(GCC_PATH)/$(PREFIX)gcc
AS := $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP := $(GCC_PATH)/$(PREFIX)objcopy
SZ := $(GCC_PATH)/$(PREFIX)size
else
CC := $(PREFIX)gcc
AS := $(PREFIX)gcc -x assembler-with-cpp
CP := $(PREFIX)objcopy
SZ := $(PREFIX)size
endif

HEX := $(CP) -O ihex
BIN := $(CP) -O binary -S

################################################################################
# Project Configuration
################################################################################
PROJECT_NAME  := pu
BOOT_TARGET   := $(PROJECT_NAME)_boot
APP_TARGET    := $(PROJECT_NAME)_app
BOOT_BUILD_DIR := $(CURRENT_DIR)/../$(BOOT_TARGET)_build
APP_BUILD_DIR := $(CURRENT_DIR)/../$(APP_TARGET)_build
MERGE_OUTPUT := $(APP_BUILD_DIR)/merge.bin
DEBUG := 1
OPT   := -Og

# Define the base flash address
FLASH_BASE_ADDRESS := 0x08000000

# Define the start address of the application in flash memory
APP_START_ADDRESS := 0x8000

# Define the application link address
APP_LINK_ADDRESS := $(shell printf "0x%X" $$(($(APP_START_ADDRESS) + $(FLASH_BASE_ADDRESS))))

# Define the application size
APP_LINK_SIZE := 0x38000

# Define the back application link address
BACK_APP_LINK_ADDRESS := $(shell printf "0x%X" $$(($(APP_START_ADDRESS) + $(FLASH_BASE_ADDRESS) + $(APP_LINK_SIZE))))

# Define the block size for padding
BLOCK_SIZE := 2048

# Define storage upgrade flag address
UPGRADE_FLAG_ADDRESS := 0x08078000

# Define application size (block number)
UPGRADE_BLOCK_NUM_ADDRESS := $(shell printf "0x%X" $$(($(UPGRADE_FLAG_ADDRESS) + 4)))

# Define the application CRC32 address
UPGRADE_CRC32_ADDRESS := 0x08078008

# Define the application Type address
UPGRADE_APPLICATION_TYPE_ADDRESS := 0x08078800

# Define the git commit id
GIT_COMMIT_ID := $(shell git rev-parse --short HEAD)

# Define the build time
BUILD_TIME := $(shell date "+%Y%m%d%H%M%S")

################################################################################
# Build Flags
################################################################################
CPU        := -mcpu=cortex-m4
FPU        := -mfpu=fpv4-sp-d16
FLOAT_ABI  := -mfloat-abi=hard
MCU        := $(CPU) -mthumb $(FPU) $(FLOAT_ABI)

C_DEFS  := -DUSE_STDPERIPH_DRIVER -DGD32F30X_HD -DAPP_START_ADDRESS=$(APP_START_ADDRESS) -DBLOCK_SIZE=$(BLOCK_SIZE) -DUPGRADE_FLAG_ADDRESS=$(UPGRADE_FLAG_ADDRESS)
C_DEFS  += -DUPGRADE_BLOCK_NUM_ADDRESS=$(UPGRADE_BLOCK_NUM_ADDRESS) -DUPGRADE_CRC32_ADDRESS=$(UPGRADE_CRC32_ADDRESS)
C_DEFS  += -DBACK_APP_LINK_ADDRESS=$(BACK_APP_LINK_ADDRESS)
C_DEFS  += -DUPGRADE_APPLICATION_TYPE_ADDRESS=$(UPGRADE_APPLICATION_TYPE_ADDRESS)
C_DEFS	+= -DGIT_COMMIT_ID=\"$(GIT_COMMIT_ID)\" -DBUILD_TIME=\"$(BUILD_TIME)\"

C_INCLUDES := \
    -I$(CURRENT_DIR)/firmware/gd32f30x/firmware/cmsis \
    -I$(CURRENT_DIR)/firmware/gd32f30x/firmware/gd32f30x_standard_peripheral/include \
    -I$(CURRENT_DIR)/application/common/inc \
    -I$(CURRENT_DIR)/application/registers \
	-I$(CURRENT_DIR)/application/drive \
	-I$(CURRENT_DIR)/application/fal/inc \
	-I$(CURRENT_DIR)/application/flashdb/inc \
	-I$(CURRENT_DIR)/application/bq76952 \
	-I$(CURRENT_DIR)/application/dataprocess

LIBS := -lc -lm -lnosys
ASFLAGS := $(MCU) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS  := $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -g -gdwarf-2

BOOT_LDSCRIPT := $(CURRENT_DIR)/ldscripts/gd32f303_boot_flash.ld
APP_LDSCRIPT  := $(CURRENT_DIR)/ldscripts/gd32f303_app_flash.ld
BOOT_LDFLAGS  := $(MCU) -specs=nosys.specs -Wl,--defsym=FLASH_BASE_ADDRESS=$(FLASH_BASE_ADDRESS) -T$(BOOT_LDSCRIPT) -Wl,-Map=$(BOOT_BUILD_DIR)/$(BOOT_TARGET).map,--cref -Wl,--gc-sections
APP_LDFLAGS   := $(MCU) -specs=nosys.specs -Wl,--defsym=APP_LINK_ADDRESS=$(APP_LINK_ADDRESS) -T$(APP_LDSCRIPT) -Wl,-Map=$(APP_BUILD_DIR)/$(APP_TARGET).map,--cref -Wl,--gc-sections

################################################################################
# Source Files
################################################################################
COMMON_SOURCES := \
    $(wildcard $(CURRENT_DIR)/firmware/gd32f30x/firmware/cmsis/*.c) \
    $(wildcard $(CURRENT_DIR)/firmware/gd32f30x/firmware/gd32f30x_standard_peripheral/source/*.c) \
	$(wildcard $(CURRENT_DIR)/application/common/src/*.c)

BOOT_SOURCES := \
    $(wildcard $(CURRENT_DIR)/application/upgrade/*.c)

APP_SOURCES := \
    $(wildcard $(CURRENT_DIR)/application/registers/*.c) \
    $(wildcard $(CURRENT_DIR)/application/dataprocess/*.c) \
	$(wildcard $(CURRENT_DIR)/application/primary/*.c) \
	$(wildcard $(CURRENT_DIR)/application/drive/*.c) \
	$(wildcard $(CURRENT_DIR)/application/fal/src/*.c) \
	$(wildcard $(CURRENT_DIR)/application/flashdb/src/*.c) \
	$(wildcard $(CURRENT_DIR)/application/bq76952/*.c)

ASM_SOURCE := $(CURRENT_DIR)/firmware/gd32f30x/firmware/cmsis/gcc_startup/startup_gd32f30x_hd.s

################################################################################
# Object Files and Dependencies
################################################################################
BOOT_OBJECTS := $(addprefix $(BOOT_BUILD_DIR)/,$(notdir $(BOOT_SOURCES:.c=.o)))
BOOT_OBJECTS += $(addprefix $(BOOT_BUILD_DIR)/,$(notdir $(COMMON_SOURCES:.c=.o)))

APP_OBJECTS  := $(addprefix $(APP_BUILD_DIR)/,$(notdir $(APP_SOURCES:.c=.o)))
APP_OBJECTS  += $(addprefix $(APP_BUILD_DIR)/,$(notdir $(COMMON_SOURCES:.c=.o)))

ASM_OBJECT := $(BOOT_BUILD_DIR)/$(notdir $(ASM_SOURCE:.s=.o))

vpath %.c $(sort $(dir $(COMMON_SOURCES) $(BOOT_SOURCES) $(APP_SOURCES)))
vpath %.s $(dir $(ASM_SOURCE))

################################################################################
# Build Rules
################################################################################
all: boot app merge

boot: $(BOOT_BUILD_DIR)/$(BOOT_TARGET).elf

$(BOOT_BUILD_DIR)/%.o: %.c Makefile | $(BOOT_BUILD_DIR)
	$(CC) -c $(CFLAGS) -MMD -MP -MF"$(@:.o=.d)" $< -o $@

$(BOOT_BUILD_DIR)/%.o: %.s Makefile | $(BOOT_BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BOOT_BUILD_DIR)/$(BOOT_TARGET).elf: $(BOOT_OBJECTS) $(ASM_OBJECT)
	$(CC) $^ $(BOOT_LDFLAGS) -o $@
	$(SZ) $@
	$(HEX) $(BOOT_BUILD_DIR)/$(BOOT_TARGET).elf $(BOOT_BUILD_DIR)/$(BOOT_TARGET).hex
	$(BIN) $(BOOT_BUILD_DIR)/$(BOOT_TARGET).elf $(BOOT_BUILD_DIR)/$(BOOT_TARGET).bin
	@echo "Congratulations $(BOOT_BUILD_DIR)/$(BOOT_TARGET).bin compiled successfully."

$(BOOT_BUILD_DIR):
	mkdir -p $@

app: $(APP_BUILD_DIR)/$(APP_TARGET).elf

$(APP_BUILD_DIR)/%.o: %.c Makefile | $(APP_BUILD_DIR)
	$(CC) -c $(CFLAGS) -MMD -MP -MF"$(@:.o=.d)" $< -o $@

$(APP_BUILD_DIR)/%.o: %.s Makefile | $(APP_BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(APP_BUILD_DIR)/$(APP_TARGET).elf: $(APP_OBJECTS) $(ASM_OBJECT)
	$(CC) $^ $(APP_LDFLAGS) -o $@
	$(SZ) $@
	$(HEX) $(APP_BUILD_DIR)/$(APP_TARGET).elf $(APP_BUILD_DIR)/$(APP_TARGET).hex
	$(BIN) $(APP_BUILD_DIR)/$(APP_TARGET).elf $(APP_BUILD_DIR)/$(APP_TARGET).bin
	@echo "Congratulations $(APP_BUILD_DIR)/$(APP_TARGET).bin compiled successfully."

$(APP_BUILD_DIR):
	mkdir -p $@

merge: $(BOOT_BUILD_DIR)/$(BOOT_TARGET).bin $(APP_BUILD_DIR)/$(APP_TARGET).bin
	@file_size=$$(stat -c %s $(APP_BUILD_DIR)/$(APP_TARGET).bin); \
	if [ -z "$$file_size" ]; then \
		echo "Error: Unable to determine file size."; \
		exit 1; \
	fi; \
	remainder=$$(($$file_size % $(BLOCK_SIZE))); \
	output_file="$(APP_BUILD_DIR)/$(APP_TARGET)_$(BLOCK_SIZE).bin"; \
	cp "$(APP_BUILD_DIR)/$(APP_TARGET).bin" "$$output_file"; \
	if [ $$remainder -ne 0 ]; then \
		pad_size=$$(( $(BLOCK_SIZE) - $$remainder )); \
		dd if=/dev/zero bs=1 count=$$pad_size >> "$$output_file" status=none; \
	fi; \
	echo "Build BLOCK FILE: $$output_file success!!!"; \
	SIZE1=$$(stat -c %s $(BOOT_BUILD_DIR)/$(BOOT_TARGET).bin); \
	PADDING=$$(( $(APP_START_ADDRESS) - $$SIZE1 )); \
	if [ $$PADDING -lt 0 ]; then \
		echo "Error: BOOT_TARGET binary size exceeds $(APP_START_ADDRESS) bytes."; \
		exit 1; \
	fi; \
	dd if=/dev/zero bs=1 count=$$PADDING 2>/dev/null | tr '\0' '\xff' > $(APP_BUILD_DIR)/padding.bin; \
	cat $(BOOT_BUILD_DIR)/$(BOOT_TARGET).bin $(APP_BUILD_DIR)/padding.bin "$$output_file" > $(MERGE_OUTPUT); \
	rm -f $(APP_BUILD_DIR)/padding.bin; \
	echo "Merged binary created: $(MERGE_OUTPUT) success!!!"; \

clean:
	rm -rf $(BOOT_BUILD_DIR) $(APP_BUILD_DIR)

################################################################################
# Flash Firmware
################################################################################
JFLASH        := /usr/bin/JFlash
JFLASH_LOG    := $(HOME)/jflash_log.log
JFLASH_CONFIG := $(CURRENT_DIR)/jflash/gd32f303.jflash

burn:
	@echo "Burn firmware."
	@$(JFLASH) -openprj"$(JFLASH_CONFIG)" -open"$(MERGE_OUTPUT)","$(FLASH_BASE_ADDRESS)" -auto -jflashlog"$(JFLASH_LOG)" -min -exit
	@echo "Burn firmware success."

################################################################################
# Align Sources
################################################################################
INCLUDE_DIR   := $(foreach cheader,$(C_INCLUDES),$(patsubst -I%,%,$(cheader)))
HEADER_FILES  := $(foreach header,$(INCLUDE_DIR),$(wildcard $(header)/*.h))
ALIGN_SOURCES := $(COMMON_SOURCES) \
				 $(APP_SOURCES) \
				 $(BOOT_SOURCES)

align:
	$(foreach csource,$(ALIGN_SOURCES),$(shell dos2unix $(csource) >/dev/null 2>&1))
	$(foreach hsource,$(HEADER_FILES),$(shell dos2unix $(hsource) >/dev/null 2>&1))
	$(foreach csource,$(ALIGN_SOURCES),$(shell astyle --style=linux --suffix=none $(csource) >/dev/null))
	$(foreach hsource,$(HEADER_FILES),$(shell astyle --style=linux --suffix=none $(hsource) >/dev/null))

################################################################################
# Include Dependencies
################################################################################
-include $(wildcard $(BOOT_BUILD_DIR)/*.d $(APP_BUILD_DIR)/*.d)

.PHONY: all boot app merge clean burn align
