################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LVGL/lvgl/src/widgets/lv_arc.c \
../LVGL/lvgl/src/widgets/lv_bar.c \
../LVGL/lvgl/src/widgets/lv_btn.c \
../LVGL/lvgl/src/widgets/lv_btnmatrix.c \
../LVGL/lvgl/src/widgets/lv_canvas.c \
../LVGL/lvgl/src/widgets/lv_checkbox.c \
../LVGL/lvgl/src/widgets/lv_dropdown.c \
../LVGL/lvgl/src/widgets/lv_img.c \
../LVGL/lvgl/src/widgets/lv_label.c \
../LVGL/lvgl/src/widgets/lv_line.c \
../LVGL/lvgl/src/widgets/lv_objx_templ.c \
../LVGL/lvgl/src/widgets/lv_roller.c \
../LVGL/lvgl/src/widgets/lv_slider.c \
../LVGL/lvgl/src/widgets/lv_switch.c \
../LVGL/lvgl/src/widgets/lv_table.c \
../LVGL/lvgl/src/widgets/lv_textarea.c 

OBJS += \
./LVGL/lvgl/src/widgets/lv_arc.o \
./LVGL/lvgl/src/widgets/lv_bar.o \
./LVGL/lvgl/src/widgets/lv_btn.o \
./LVGL/lvgl/src/widgets/lv_btnmatrix.o \
./LVGL/lvgl/src/widgets/lv_canvas.o \
./LVGL/lvgl/src/widgets/lv_checkbox.o \
./LVGL/lvgl/src/widgets/lv_dropdown.o \
./LVGL/lvgl/src/widgets/lv_img.o \
./LVGL/lvgl/src/widgets/lv_label.o \
./LVGL/lvgl/src/widgets/lv_line.o \
./LVGL/lvgl/src/widgets/lv_objx_templ.o \
./LVGL/lvgl/src/widgets/lv_roller.o \
./LVGL/lvgl/src/widgets/lv_slider.o \
./LVGL/lvgl/src/widgets/lv_switch.o \
./LVGL/lvgl/src/widgets/lv_table.o \
./LVGL/lvgl/src/widgets/lv_textarea.o 

C_DEPS += \
./LVGL/lvgl/src/widgets/lv_arc.d \
./LVGL/lvgl/src/widgets/lv_bar.d \
./LVGL/lvgl/src/widgets/lv_btn.d \
./LVGL/lvgl/src/widgets/lv_btnmatrix.d \
./LVGL/lvgl/src/widgets/lv_canvas.d \
./LVGL/lvgl/src/widgets/lv_checkbox.d \
./LVGL/lvgl/src/widgets/lv_dropdown.d \
./LVGL/lvgl/src/widgets/lv_img.d \
./LVGL/lvgl/src/widgets/lv_label.d \
./LVGL/lvgl/src/widgets/lv_line.d \
./LVGL/lvgl/src/widgets/lv_objx_templ.d \
./LVGL/lvgl/src/widgets/lv_roller.d \
./LVGL/lvgl/src/widgets/lv_slider.d \
./LVGL/lvgl/src/widgets/lv_switch.d \
./LVGL/lvgl/src/widgets/lv_table.d \
./LVGL/lvgl/src/widgets/lv_textarea.d 


# Each subdirectory must supply rules for building sources it contributes
LVGL/lvgl/src/widgets/%.o: ../LVGL/lvgl/src/widgets/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\Works\NSCAM\RVMSIS" -I"E:\Works\NSCAM\FATFS" -I"E:\Works\NSCAM\Sensor" -I"E:\Works\NSCAM\LVGL" -I"E:\Works\NSCAM\SD" -I"E:\Works\NSCAM\User" -I"E:\Works\NSCAM\LCD\inc" -I"E:\Works\NSCAM\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

