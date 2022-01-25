################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/CPDev_XCPcodes/2020/32bitAlign/ShipSim_32ARM.c \
../Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test1_32ARM.c \
../Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test2_32ARM.c \
../Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test3_32ARM.c 

C_DEPS += \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/ShipSim_32ARM.d \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test1_32ARM.d \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test2_32ARM.d \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test3_32ARM.d 

OBJS += \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/ShipSim_32ARM.o \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test1_32ARM.o \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test2_32ARM.o \
./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test3_32ARM.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/CPDev_XCPcodes/2020/32bitAlign/%.o: ../Core/Lib/CPDev_XCPcodes/2020/32bitAlign/%.c Core/Lib/CPDev_XCPcodes/2020/32bitAlign/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-CPDev_XCPcodes-2f-2020-2f-32bitAlign

clean-Core-2f-Lib-2f-CPDev_XCPcodes-2f-2020-2f-32bitAlign:
	-$(RM) ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/ShipSim_32ARM.d ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/ShipSim_32ARM.o ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test1_32ARM.d ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test1_32ARM.o ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test2_32ARM.d ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test2_32ARM.o ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test3_32ARM.d ./Core/Lib/CPDev_XCPcodes/2020/32bitAlign/test3_32ARM.o

.PHONY: clean-Core-2f-Lib-2f-CPDev_XCPcodes-2f-2020-2f-32bitAlign

