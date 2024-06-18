################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LVGL/lvgl/src/draw/lv_draw_arc.c \
../LVGL/lvgl/src/draw/lv_draw_blend.c \
../LVGL/lvgl/src/draw/lv_draw_img.c \
../LVGL/lvgl/src/draw/lv_draw_label.c \
../LVGL/lvgl/src/draw/lv_draw_line.c \
../LVGL/lvgl/src/draw/lv_draw_mask.c \
../LVGL/lvgl/src/draw/lv_draw_rect.c \
../LVGL/lvgl/src/draw/lv_draw_triangle.c \
../LVGL/lvgl/src/draw/lv_img_buf.c \
../LVGL/lvgl/src/draw/lv_img_cache.c \
../LVGL/lvgl/src/draw/lv_img_decoder.c 

OBJS += \
./LVGL/lvgl/src/draw/lv_draw_arc.o \
./LVGL/lvgl/src/draw/lv_draw_blend.o \
./LVGL/lvgl/src/draw/lv_draw_img.o \
./LVGL/lvgl/src/draw/lv_draw_label.o \
./LVGL/lvgl/src/draw/lv_draw_line.o \
./LVGL/lvgl/src/draw/lv_draw_mask.o \
./LVGL/lvgl/src/draw/lv_draw_rect.o \
./LVGL/lvgl/src/draw/lv_draw_triangle.o \
./LVGL/lvgl/src/draw/lv_img_buf.o \
./LVGL/lvgl/src/draw/lv_img_cache.o \
./LVGL/lvgl/src/draw/lv_img_decoder.o 

C_DEPS += \
./LVGL/lvgl/src/draw/lv_draw_arc.d \
./LVGL/lvgl/src/draw/lv_draw_blend.d \
./LVGL/lvgl/src/draw/lv_draw_img.d \
./LVGL/lvgl/src/draw/lv_draw_label.d \
./LVGL/lvgl/src/draw/lv_draw_line.d \
./LVGL/lvgl/src/draw/lv_draw_mask.d \
./LVGL/lvgl/src/draw/lv_draw_rect.d \
./LVGL/lvgl/src/draw/lv_draw_triangle.d \
./LVGL/lvgl/src/draw/lv_img_buf.d \
./LVGL/lvgl/src/draw/lv_img_cache.d \
./LVGL/lvgl/src/draw/lv_img_decoder.d 


# Each subdirectory must supply rules for building sources it contributes
LVGL/lvgl/src/draw/%.o: ../LVGL/lvgl/src/draw/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\Works\NSCAM\RVMSIS" -I"E:\Works\NSCAM\FATFS" -I"E:\Works\NSCAM\Sensor" -I"E:\Works\NSCAM\LVGL" -I"E:\Works\NSCAM\SD" -I"E:\Works\NSCAM\User" -I"E:\Works\NSCAM\LCD\inc" -I"E:\Works\NSCAM\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

