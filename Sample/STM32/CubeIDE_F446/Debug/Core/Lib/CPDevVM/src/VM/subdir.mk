################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Lib/CPDevVM/src/VM/vm.cpp 

OBJS += \
./Core/Lib/CPDevVM/src/VM/vm.o 

CPP_DEPS += \
./Core/Lib/CPDevVM/src/VM/vm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/CPDevVM/src/VM/%.o: ../Core/Lib/CPDevVM/src/VM/%.cpp Core/Lib/CPDevVM/src/VM/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-CPDevVM-2f-src-2f-VM

clean-Core-2f-Lib-2f-CPDevVM-2f-src-2f-VM:
	-$(RM) ./Core/Lib/CPDevVM/src/VM/vm.d ./Core/Lib/CPDevVM/src/VM/vm.o

.PHONY: clean-Core-2f-Lib-2f-CPDevVM-2f-src-2f-VM

