################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APIs/log.c 

OBJS += \
./APIs/log.o 

C_DEPS += \
./APIs/log.d 


# Each subdirectory must supply rules for building sources it contributes
APIs/%.o APIs/%.su: ../APIs/%.c APIs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"D:/Documents/stm32-FreeRTOS-examples/Nucleo-F103RB/00_itm_project_config/APIs" -I"D:/Documents/stm32-FreeRTOS-examples/Nucleo-F103RB/00_itm_project_config/Middlewares/Third_Party/SEGGER" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-APIs

clean-APIs:
	-$(RM) ./APIs/log.d ./APIs/log.o ./APIs/log.su

.PHONY: clean-APIs

