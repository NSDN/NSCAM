################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LVGL/lvgl/src/core/lv_disp.c \
../LVGL/lvgl/src/core/lv_event.c \
../LVGL/lvgl/src/core/lv_group.c \
../LVGL/lvgl/src/core/lv_indev.c \
../LVGL/lvgl/src/core/lv_indev_scroll.c \
../LVGL/lvgl/src/core/lv_obj.c \
../LVGL/lvgl/src/core/lv_obj_class.c \
../LVGL/lvgl/src/core/lv_obj_draw.c \
../LVGL/lvgl/src/core/lv_obj_pos.c \
../LVGL/lvgl/src/core/lv_obj_scroll.c \
../LVGL/lvgl/src/core/lv_obj_style.c \
../LVGL/lvgl/src/core/lv_obj_style_gen.c \
../LVGL/lvgl/src/core/lv_obj_tree.c \
../LVGL/lvgl/src/core/lv_refr.c \
../LVGL/lvgl/src/core/lv_theme.c 

OBJS += \
./LVGL/lvgl/src/core/lv_disp.o \
./LVGL/lvgl/src/core/lv_event.o \
./LVGL/lvgl/src/core/lv_group.o \
./LVGL/lvgl/src/core/lv_indev.o \
./LVGL/lvgl/src/core/lv_indev_scroll.o \
./LVGL/lvgl/src/core/lv_obj.o \
./LVGL/lvgl/src/core/lv_obj_class.o \
./LVGL/lvgl/src/core/lv_obj_draw.o \
./LVGL/lvgl/src/core/lv_obj_pos.o \
./LVGL/lvgl/src/core/lv_obj_scroll.o \
./LVGL/lvgl/src/core/lv_obj_style.o \
./LVGL/lvgl/src/core/lv_obj_style_gen.o \
./LVGL/lvgl/src/core/lv_obj_tree.o \
./LVGL/lvgl/src/core/lv_refr.o \
./LVGL/lvgl/src/core/lv_theme.o 

C_DEPS += \
./LVGL/lvgl/src/core/lv_disp.d \
./LVGL/lvgl/src/core/lv_event.d \
./LVGL/lvgl/src/core/lv_group.d \
./LVGL/lvgl/src/core/lv_indev.d \
./LVGL/lvgl/src/core/lv_indev_scroll.d \
./LVGL/lvgl/src/core/lv_obj.d \
./LVGL/lvgl/src/core/lv_obj_class.d \
./LVGL/lvgl/src/core/lv_obj_draw.d \
./LVGL/lvgl/src/core/lv_obj_pos.d \
./LVGL/lvgl/src/core/lv_obj_scroll.d \
./LVGL/lvgl/src/core/lv_obj_style.d \
./LVGL/lvgl/src/core/lv_obj_style_gen.d \
./LVGL/lvgl/src/core/lv_obj_tree.d \
./LVGL/lvgl/src/core/lv_refr.d \
./LVGL/lvgl/src/core/lv_theme.d 


# Each subdirectory must supply rules for building sources it contributes
LVGL/lvgl/src/core/%.o: ../LVGL/lvgl/src/core/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\Works\NSCAM\RVMSIS" -I"E:\Works\NSCAM\FATFS" -I"E:\Works\NSCAM\Sensor" -I"E:\Works\NSCAM\LVGL" -I"E:\Works\NSCAM\SD" -I"E:\Works\NSCAM\User" -I"E:\Works\NSCAM\LCD\inc" -I"E:\Works\NSCAM\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

