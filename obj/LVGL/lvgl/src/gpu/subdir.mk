################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LVGL/lvgl/src/gpu/lv_gpu_nxp_pxp.c \
../LVGL/lvgl/src/gpu/lv_gpu_nxp_pxp_osa.c \
../LVGL/lvgl/src/gpu/lv_gpu_nxp_vglite.c \
../LVGL/lvgl/src/gpu/lv_gpu_stm32_dma2d.c 

OBJS += \
./LVGL/lvgl/src/gpu/lv_gpu_nxp_pxp.o \
./LVGL/lvgl/src/gpu/lv_gpu_nxp_pxp_osa.o \
./LVGL/lvgl/src/gpu/lv_gpu_nxp_vglite.o \
./LVGL/lvgl/src/gpu/lv_gpu_stm32_dma2d.o 

C_DEPS += \
./LVGL/lvgl/src/gpu/lv_gpu_nxp_pxp.d \
./LVGL/lvgl/src/gpu/lv_gpu_nxp_pxp_osa.d \
./LVGL/lvgl/src/gpu/lv_gpu_nxp_vglite.d \
./LVGL/lvgl/src/gpu/lv_gpu_stm32_dma2d.d 


# Each subdirectory must supply rules for building sources it contributes
LVGL/lvgl/src/gpu/%.o: ../LVGL/lvgl/src/gpu/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\Works\NSCAM\RVMSIS" -I"E:\Works\NSCAM\FATFS" -I"E:\Works\NSCAM\Sensor" -I"E:\Works\NSCAM\LVGL" -I"E:\Works\NSCAM\SD" -I"E:\Works\NSCAM\User" -I"E:\Works\NSCAM\LCD\inc" -I"E:\Works\NSCAM\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

