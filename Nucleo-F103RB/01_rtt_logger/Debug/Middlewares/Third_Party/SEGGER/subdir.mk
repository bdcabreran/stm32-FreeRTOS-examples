################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/SEGGER/SEGGER_RTT.c \
../Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_IAR.c \
../Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_KEIL.c \
../Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_SES.c \
../Middlewares/Third_Party/SEGGER/SEGGER_RTT_printf.c \
../Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW.c \
../Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_Config_FreeRTOS.c \
../Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_FreeRTOS.c 

S_UPPER_SRCS += \
../Middlewares/Third_Party/SEGGER/SEGGER_RTT_ASM_ARMv7M.S 

OBJS += \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT.o \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_ASM_ARMv7M.o \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_IAR.o \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_KEIL.o \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_SES.o \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_printf.o \
./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW.o \
./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_Config_FreeRTOS.o \
./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_FreeRTOS.o 

S_UPPER_DEPS += \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_ASM_ARMv7M.d 

C_DEPS += \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT.d \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_IAR.d \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_KEIL.d \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_SES.d \
./Middlewares/Third_Party/SEGGER/SEGGER_RTT_printf.d \
./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW.d \
./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_Config_FreeRTOS.d \
./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_FreeRTOS.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/SEGGER/%.o Middlewares/Third_Party/SEGGER/%.su: ../Middlewares/Third_Party/SEGGER/%.c Middlewares/Third_Party/SEGGER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/Third_Party/SEGGER/%.o: ../Middlewares/Third_Party/SEGGER/%.S Middlewares/Third_Party/SEGGER/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Middlewares-2f-Third_Party-2f-SEGGER

clean-Middlewares-2f-Third_Party-2f-SEGGER:
	-$(RM) ./Middlewares/Third_Party/SEGGER/SEGGER_RTT.d ./Middlewares/Third_Party/SEGGER/SEGGER_RTT.o ./Middlewares/Third_Party/SEGGER/SEGGER_RTT.su ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_ASM_ARMv7M.d ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_ASM_ARMv7M.o ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_IAR.d ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_IAR.o ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_IAR.su ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_KEIL.d ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_KEIL.o ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_KEIL.su ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_SES.d ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_SES.o ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_Syscalls_SES.su ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_printf.d ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_printf.o ./Middlewares/Third_Party/SEGGER/SEGGER_RTT_printf.su ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW.d ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW.o ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW.su ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_Config_FreeRTOS.d ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_Config_FreeRTOS.o ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_Config_FreeRTOS.su ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_FreeRTOS.d ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_FreeRTOS.o ./Middlewares/Third_Party/SEGGER/SEGGER_SYSVIEW_FreeRTOS.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-SEGGER

