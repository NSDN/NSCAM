################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Sensor/subdir.mk
-include SD/subdir.mk
-include RVMSIS/subdir.mk
-include Peripheral/src/subdir.mk
-include LVGL/lvgl/src/widgets/subdir.mk
-include LVGL/lvgl/src/misc/subdir.mk
-include LVGL/lvgl/src/hal/subdir.mk
-include LVGL/lvgl/src/gpu/subdir.mk
-include LVGL/lvgl/src/font/subdir.mk
-include LVGL/lvgl/src/extra/widgets/win/subdir.mk
-include LVGL/lvgl/src/extra/widgets/tileview/subdir.mk
-include LVGL/lvgl/src/extra/widgets/tabview/subdir.mk
-include LVGL/lvgl/src/extra/widgets/spinner/subdir.mk
-include LVGL/lvgl/src/extra/widgets/spinbox/subdir.mk
-include LVGL/lvgl/src/extra/widgets/span/subdir.mk
-include LVGL/lvgl/src/extra/widgets/msgbox/subdir.mk
-include LVGL/lvgl/src/extra/widgets/meter/subdir.mk
-include LVGL/lvgl/src/extra/widgets/list/subdir.mk
-include LVGL/lvgl/src/extra/widgets/led/subdir.mk
-include LVGL/lvgl/src/extra/widgets/keyboard/subdir.mk
-include LVGL/lvgl/src/extra/widgets/imgbtn/subdir.mk
-include LVGL/lvgl/src/extra/widgets/colorwheel/subdir.mk
-include LVGL/lvgl/src/extra/widgets/chart/subdir.mk
-include LVGL/lvgl/src/extra/widgets/calendar/subdir.mk
-include LVGL/lvgl/src/extra/widgets/animimg/subdir.mk
-include LVGL/lvgl/src/extra/themes/mono/subdir.mk
-include LVGL/lvgl/src/extra/themes/default/subdir.mk
-include LVGL/lvgl/src/extra/themes/basic/subdir.mk
-include LVGL/lvgl/src/extra/layouts/grid/subdir.mk
-include LVGL/lvgl/src/extra/layouts/flex/subdir.mk
-include LVGL/lvgl/src/extra/subdir.mk
-include LVGL/lvgl/src/draw/subdir.mk
-include LVGL/lvgl/src/core/subdir.mk
-include LVGL/subdir.mk
-include LCD/subdir.mk
-include FATFS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
NSCAM.hex \

SECONDARY_LIST += \
NSCAM.lst \

SECONDARY_SIZE += \
NSCAM.siz \


# All Target
all: NSCAM.elf secondary-outputs

# Tool invocations
NSCAM.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T .ld -nostartfiles -Xlinker --gc-sections -L../ -Wl,-Map,"NSCAM.map" --specs=nano.specs --specs=nosys.specs -o "NSCAM.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
NSCAM.hex: NSCAM.elf
	@	riscv-none-embed-objcopy -O ihex "NSCAM.elf"  "NSCAM.hex"
	@	@
NSCAM.lst: NSCAM.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "NSCAM.elf" > "NSCAM.lst"
	@	@
NSCAM.siz: NSCAM.elf
	@	riscv-none-embed-size --format=berkeley "NSCAM.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) NSCAM.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
