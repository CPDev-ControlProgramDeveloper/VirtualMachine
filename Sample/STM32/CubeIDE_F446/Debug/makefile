################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Core/Lib/CPDev_XCPcodes/2020/32bitAlign/subdir.mk
-include Core/Lib/CPDev_XCPcodes/2020/32bit/2020_ShipSim/subdir.mk
-include Core/Lib/CPDev_XCPcodes/2020/16bit/test03/subdir.mk
-include Core/Lib/CPDev_XCPcodes/2020/16bit/test02/subdir.mk
-include Core/Lib/CPDev_XCPcodes/2020/16bit/test01/subdir.mk
-include Core/Lib/CPDev_XCPcodes/2020/16bit/ShipSim/subdir.mk
-include Core/Lib/CPDev_XCPcodes/2020/16bit/BlinkRS/subdir.mk
-include Core/Lib/CPDevVM/src/VM/vmfunc/subdir.mk
-include Core/Lib/CPDevVM/src/VM/subdir.mk
-include Core/Lib/CPDevVM/src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := CPDevObj_STM32
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
CPDevObj_STM32.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
CPDevObj_STM32.list \

OBJCOPY_BIN += \
CPDevObj_STM32.bin \


# All Target
all: main-build

# Main-build Target
main-build: CPDevObj_STM32.elf secondary-outputs

# Tool invocations
CPDevObj_STM32.elf: $(OBJS) $(USER_OBJS) C:\Users\Marcin\OneDrive\PROJEKTY\12_EMBEDDED\140_CPDev_Test\CPDevObj_STM32\STM32F446RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "CPDevObj_STM32.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Marcin\OneDrive\PROJEKTY\12_EMBEDDED\140_CPDev_Test\CPDevObj_STM32\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="CPDevObj_STM32.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

CPDevObj_STM32.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "CPDevObj_STM32.list"
	@echo 'Finished building: $@'
	@echo ' '

CPDevObj_STM32.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "CPDevObj_STM32.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) CPDevObj_STM32.bin CPDevObj_STM32.elf CPDevObj_STM32.list default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
