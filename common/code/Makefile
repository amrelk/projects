# worlds worst multiline comment
ifeq (a, b)

WPI's custom stm32 cubemx-based build system!

The intent here is that you can include this in your board's `<boardname>/<code>/Makefile`

This file expects a certain file layout, example shown for the segmentboard board
.
├── common
│   └── code
│       ├── inc (this is C headers exclusively)
│       │   ├── macros.h
│       │   ├── etl/
│       │   └── etl_profile.h
│       ├── Makefile (this file!)
│       └── src (this is c++ headers and source files)
│           ├── common.cpp
│           └── common.hpp
│
└── segmentboard
    └── code
        ├── code.ioc
        ├── Makefile
        └── src
            ├── main.cpp
            └── main.hpp


Things you **need** to set in your makefile:
Things you can override:
- DEBUG             Set to '1' to enable debug, or '0' to disable debug. 1 by default

- SILENCE           Set to "" to print compiler commands. Don't set to anything else

- TARGET            Set to the name of your *.ioc file. The default is "code".

- PREFIX            This is the prefix of your arm-none-eabi compiler suite.
                    Might be useful to set if you have fucked paths.
                    Default "arm-none-eabi-", which will rely on your PATH variable

- DEFS              These are compiler flags passed to AS, GCC, and G++. Intended for -DFOO=bar definitions

- CUBEMX_COMMAND    The command to run STM32CubeMX. You may want `java -jar <your path to cube mx>.
                    Do not fucking put this in the board-specific makefile, instead add it to your ~/.bashrc
                    Defaults to 'stm32cubemx'

Commansd you can run:
- make all          Builds everything (default)
- make stm32        Generates all STM32CubeMX files
- make bear         Generates a compile_commands.json file
- make clean        Cleans up `build/`. Does not delete generated STM32CubeMX files.
- make fullclean    Cleans up everything including STM32CubeMX generated files and bear file

Defines that might be useful:
	-DEV24_EXCLUDE_CAN    exclude common code that references the CAN 

endif


# useless things lol
RED=$(shell tput setaf 1)
GREEN=$(shell tput setaf 2)
NORMAL=$(shell tput sgr0)

SILENCE ?= @

TARGET ?= code

DEBUG ?= 1

BUILD_DIR = build

COMMON_DIR = $(shell pwd | sed 's/projects.*//')projects/common

CHIP ?= $(shell cat $(TARGET).ioc 2>/dev/null | awk -F "=" '/Mcu.CPN/{ print $$2 }')

C_SOURCES   = $(shell find Core Drivers -name '*.c' 2>/dev/null)
ASM_SOURCES = $(shell find gen -name '*.s' 2>/dev/null)
CXX_SOURCES = $(shell find src ../../common/code/src -name '*.cpp' 2>/dev/null)

PREFIX ?= arm-none-eabi-

# basic tools
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
DB = $(PREFIX)gdb

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# to detect if you don't have a valid chip
CPU = none

ifeq ($(CHIP), STM32F042C6U7)
    CPU = -mcpu=cortex-m0
    # FPU = 
    # FLOAT-ABI = 
    C_DEFS = -DSTM32F042x6 -DUSE_FLASH
    OCD_TARGET = stm32f0x
    LDSCRIPT = $(COMMON_DIR)/ld/STM32F042C6Ux_FLASH.ld
endif

ifeq ($(CHIP), STM32F413RGT6)
    CPU = -mcpu=cortex-m4
    FPU = -mfpu=fpv4-sp-d16
    FLOAT-ABI = -mfloat-abi=hard
    C_DEFS = -DSTM32F413xx
    OCD_TARGET = stm32f4x
endif

ifeq ($(CHIP), STM32G031K8U6)
	CPU = -mcpu=cortex-m0plus
	# FPU = 
	# FLOAT-ABI =
	C_DEFS = -DSTM32G031xx
	OCD_TARGET = stm32g0x
endif


ifeq ($(CPU), none)
$(error Don't know what your chip ($(CHIP)) is - erroring)
endif

MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# set to nothing by default
DEFS ?=

C_DEFS += $(DEFS) -DETL_NO_STL
AS_DEFS = $(DEFS)
CXX_DEFS = $(C_DEFS) -std=c++2b


AS_INCLUDES =

C_INCLUDES = \
-ICore/Inc \
-I$(shell find * -wholename "Drivers/*/Inc") \
-I$(shell find * -wholename "Drivers/*/Inc/Legacy") \
-I$(shell find * -wholename "Drivers/CMSIS/Device/ST/*/Include") \
-IDrivers/CMSIS/Include \
-I../../common/code/inc

CXX_INCLUDES = $(C_INCLUDES)  -I../../common/code/src

#$(info    C_INCLUDES is $(C_INCLUDES))

WARNING_FLAGS = -Wall -Werror -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=unused-function

DEP_INFO_FLAGS = -MMD -MP -MF"$(@:%.o=%.d)"

SHARED_FLAGS =  $(WARNING_FLAGS)  -fdata-sections -ffunction-sections

# compile gcc flags
AS_FLAGS  = $(MCU) $(AS_DEFS)  $(AS_INCLUDES)  $(OPT) $(SHARED_FLAGS)

C_FLAGS   = $(MCU) $(C_DEFS)   $(C_INCLUDES)   $(OPT) $(SHARED_FLAGS) $(DEP_INFO_FLAGS)


CXX_FLAGS = $(MCU) $(CXX_DEFS) $(CXX_INCLUDES) $(OPT) $(SHARED_FLAGS) $(WARNING_FLAGS) $(DEP_INFO_FLAGS)
CXX_FLAGS += -fno-exceptions

ifeq ($(DEBUG), 1)
C_FLAGS += -g -gdwarf-2 -Og
CXX_FLAGS += -g -gdwarf-2 -Og
else
C_FLAGS += -03
CXX_FLAGS += -03
endif


LDSCRIPT ?= $(shell find gen/*.ld)


# libraries

LIBS = -lc -lm -lnosys
LD_FLAGS = $(MCU) -specs=nano.specs -specs=nosys.specs -T$(LDSCRIPT) $(LIBS)\
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
	-Wl,--gc-sections \
	-Wl,--no-warn-rwx-segment





################################
## actually build the things! ##
################################

# list of C objects
OBJECTS = $(addprefix $(BUILD_DIR)/c/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of ASM objects
OBJECTS += $(addprefix $(BUILD_DIR)/asm/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# list of C++ objects
OBJECTS += $(addprefix $(BUILD_DIR)/cpp/,$(notdir $(CXX_SOURCES:.cpp=.o)))
vpath %.s $(sort $(dir $(CXX_SOURCES)))

$(BUILD_DIR)/c/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "$(GREEN)[compiling]$(NORMAL) " $<
	$(SILENCE)$(CC) -c $(C_FLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/c/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/asm/%.o: gen/%.s Makefile | $(BUILD_DIR)
	@echo "$(GREEN)[compiling]$(NORMAL) " $<
	$(SILENCE)$(AS) -c $(AS_FLAGS) $< -o $@

$(BUILD_DIR)/cpp/%.o: src/%.cpp Makefile | $(BUILD_DIR)
	@echo "$(GREEN)[compiling]$(NORMAL) " $<
	$(SILENCE)$(CXX) -c $(CXX_FLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/cpp/$(notdir $(<:.cpp=.lst)) $< -o $@
# THIS IS  A DIRECT COPY PASTE OF THE LINE ABOVE, with the source changed and the debug print changed
$(BUILD_DIR)/cpp/%.o: $(COMMON_DIR)/code/src/%.cpp Makefile | $(BUILD_DIR)
	@echo "$(GREEN)[compiling]$(NORMAL) " $(shell echo "$<" | cut -c 7-)
	$(SILENCE)$(CXX) -c $(CXX_FLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/cpp/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(SILENCE)echo "$(GREEN)[compiling]$(NORMAL) " $@
	$(SILENCE)$(CC) $(OBJECTS) $(LD_FLAGS) -o $@
	$(SILENCE)$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(SILENCE)$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(SILENCE)$(BIN) $< $@
	
$(BUILD_DIR):
	$(SILENCE)mkdir $@
	$(SILENCE)mkdir $@/c
	$(SILENCE)mkdir $@/cpp
	$(SILENCE)mkdir $@/asm		


#######################################
# phony targets
#######################################
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

clean:
	@echo "$(GREEN)[Cleaning]$(NORMAL)"
	-@rm -fR $(BUILD_DIR)

fullclean:
	@echo "$(RED)[Full Cleaning]$(NORMAL)"
	-@rm -fR $(BUILD_DIR)
	-@rm -fR build Core Drivers gen .mxproject
	-@rm -fR .mxproject
	-@rm -fR compile_commands.json

CUBEMX_COMMAND ?= stm32cubemx
stm32:
	@echo -e "config load $(TARGET).ioc\nproject generate\nexit" >> cubemx_commands.tmp
	@cat cubemx_commands.tmp | $(CUBEMX_COMMAND) -q
	@-rm -f cubemx_commands.tmp

bear:
	make -f Makefile clean
	bear -- make -f Makefile -j6
	

OPENOCD_FLAGS := -f interface/stlink.cfg -f target/$(OCD_TARGET).cfg
flash: $(BUILD_DIR)/$(TARGET).elf
	openocd $(OPENOCD_FLAGS) -c 'program $(BUILD_DIR)/$(TARGET).elf verify reset exit'

target: $(BUILD_DIR)/$(TARGET).elf
	openocd $(OPENOCD_FLAGS) -c 'program $(BUILD_DIR)/$(TARGET).elf verify reset'

debug: $(BUILD_DIR)/$(TARGET).elf
	$(DB) -ex 'tar ext :3333' -ex 'break ev24::main' -ex 'monitor reset halt' -ex 'run' $<

.PHONY: clean fullclean bear stm32 all flash target debug
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/c/*.d)
-include $(wildcard $(BUILD_DIR)/cpp/*.d)
