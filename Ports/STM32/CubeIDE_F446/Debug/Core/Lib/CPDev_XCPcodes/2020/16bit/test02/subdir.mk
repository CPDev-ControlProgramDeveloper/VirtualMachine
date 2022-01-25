################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/CPDev_XCPcodes/2020/16bit/test02/test2.c 

C_DEPS += \
./Core/Lib/CPDev_XCPcodes/2020/16bit/test02/test2.d 

OBJS += \
./Core/Lib/CPDev_XCPcodes/2020/16bit/test02/test2.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/CPDev_XCPcodes/2020/16bit/test02/%.o: ../Core/Lib/CPDev_XCPcodes/2020/16bit/test02/%.c Core/Lib/CPDev_XCPcodes/2020/16bit/test02/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-CPDev_XCPcodes-2f-2020-2f-16bit-2f-test02

clean-Core-2f-Lib-2f-CPDev_XCPcodes-2f-2020-2f-16bit-2f-test02:
	-$(RM) ./Core/Lib/CPDev_XCPcodes/2020/16bit/test02/test2.d ./Core/Lib/CPDev_XCPcodes/2020/16bit/test02/test2.o

.PHONY: clean-Core-2f-Lib-2f-CPDev_XCPcodes-2f-2020-2f-16bit-2f-test02

