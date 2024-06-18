################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LVGL/lvgl/src/misc/lv_anim.c \
../LVGL/lvgl/src/misc/lv_area.c \
../LVGL/lvgl/src/misc/lv_async.c \
../LVGL/lvgl/src/misc/lv_bidi.c \
../LVGL/lvgl/src/misc/lv_color.c \
../LVGL/lvgl/src/misc/lv_fs.c \
../LVGL/lvgl/src/misc/lv_gc.c \
../LVGL/lvgl/src/misc/lv_ll.c \
../LVGL/lvgl/src/misc/lv_log.c \
../LVGL/lvgl/src/misc/lv_math.c \
../LVGL/lvgl/src/misc/lv_mem.c \
../LVGL/lvgl/src/misc/lv_printf.c \
../LVGL/lvgl/src/misc/lv_style.c \
../LVGL/lvgl/src/misc/lv_style_gen.c \
../LVGL/lvgl/src/misc/lv_templ.c \
../LVGL/lvgl/src/misc/lv_timer.c \
../LVGL/lvgl/src/misc/lv_tlsf.c \
../LVGL/lvgl/src/misc/lv_txt.c \
../LVGL/lvgl/src/misc/lv_txt_ap.c \
../LVGL/lvgl/src/misc/lv_utils.c 

OBJS += \
./LVGL/lvgl/src/misc/lv_anim.o \
./LVGL/lvgl/src/misc/lv_area.o \
./LVGL/lvgl/src/misc/lv_async.o \
./LVGL/lvgl/src/misc/lv_bidi.o \
./LVGL/lvgl/src/misc/lv_color.o \
./LVGL/lvgl/src/misc/lv_fs.o \
./LVGL/lvgl/src/misc/lv_gc.o \
./LVGL/lvgl/src/misc/lv_ll.o \
./LVGL/lvgl/src/misc/lv_log.o \
./LVGL/lvgl/src/misc/lv_math.o \
./LVGL/lvgl/src/misc/lv_mem.o \
./LVGL/lvgl/src/misc/lv_printf.o \
./LVGL/lvgl/src/misc/lv_style.o \
./LVGL/lvgl/src/misc/lv_style_gen.o \
./LVGL/lvgl/src/misc/lv_templ.o \
./LVGL/lvgl/src/misc/lv_timer.o \
./LVGL/lvgl/src/misc/lv_tlsf.o \
./LVGL/lvgl/src/misc/lv_txt.o \
./LVGL/lvgl/src/misc/lv_txt_ap.o \
./LVGL/lvgl/src/misc/lv_utils.o 

C_DEPS += \
./LVGL/lvgl/src/misc/lv_anim.d \
./LVGL/lvgl/src/misc/lv_area.d \
./LVGL/lvgl/src/misc/lv_async.d \
./LVGL/lvgl/src/misc/lv_bidi.d \
./LVGL/lvgl/src/misc/lv_color.d \
./LVGL/lvgl/src/misc/lv_fs.d \
./LVGL/lvgl/src/misc/lv_gc.d \
./LVGL/lvgl/src/misc/lv_ll.d \
./LVGL/lvgl/src/misc/lv_log.d \
./LVGL/lvgl/src/misc/lv_math.d \
./LVGL/lvgl/src/misc/lv_mem.d \
./LVGL/lvgl/src/misc/lv_printf.d \
./LVGL/lvgl/src/misc/lv_style.d \
./LVGL/lvgl/src/misc/lv_style_gen.d \
./LVGL/lvgl/src/misc/lv_templ.d \
./LVGL/lvgl/src/misc/lv_timer.d \
./LVGL/lvgl/src/misc/lv_tlsf.d \
./LVGL/lvgl/src/misc/lv_txt.d \
./LVGL/lvgl/src/misc/lv_txt_ap.d \
./LVGL/lvgl/src/misc/lv_utils.d 


# Each subdirectory must supply rules for building sources it contributes
LVGL/lvgl/src/misc/%.o: ../LVGL/lvgl/src/misc/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\Works\NSCAM\RVMSIS" -I"E:\Works\NSCAM\FATFS" -I"E:\Works\NSCAM\Sensor" -I"E:\Works\NSCAM\LVGL" -I"E:\Works\NSCAM\SD" -I"E:\Works\NSCAM\User" -I"E:\Works\NSCAM\LCD\inc" -I"E:\Works\NSCAM\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

