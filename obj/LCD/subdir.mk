################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LCD/fonts.c \
../LCD/lcd.c \
../LCD/oled.c \
../LCD/regscreen.c 

OBJS += \
./LCD/fonts.o \
./LCD/lcd.o \
./LCD/oled.o \
./LCD/regscreen.o 

C_DEPS += \
./LCD/fonts.d \
./LCD/lcd.d \
./LCD/oled.d \
./LCD/regscreen.d 


# Each subdirectory must supply rules for building sources it contributes
LCD/%.o: ../LCD/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\Works\NSCAM\RVMSIS" -I"E:\Works\NSCAM\FATFS" -I"E:\Works\NSCAM\Sensor" -I"E:\Works\NSCAM\LVGL" -I"E:\Works\NSCAM\SD" -I"E:\Works\NSCAM\User" -I"E:\Works\NSCAM\LCD\inc" -I"E:\Works\NSCAM\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

