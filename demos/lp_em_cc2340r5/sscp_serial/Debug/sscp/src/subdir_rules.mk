################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
sscp/src/%.o: ../sscp/src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/gcc-arm-none-eabi_9_3_1/bin/arm-none-eabi-gcc-9.3.1.exe" -c -mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft -D_REENT_SMALL -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_serial" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_serial/Debug" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/kernel/freertos" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/ti/posix/gcc" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/third_party/freertos/include" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/third_party/freertos/portable/GCC/ARM_CM0" -I"C:/ti/gcc-arm-none-eabi_9_3_1/arm-none-eabi/include/newlib-nano" -I"C:/ti/gcc-arm-none-eabi_9_3_1/arm-none-eabi/include" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_serial/sscp/src" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_serial/sscp/inc" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_serial/sscp" -O3 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"sscp/src/$(basename $(<F)).d_raw" -MT"$(@)" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_serial/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


