################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
sscp/src/%.o: ../sscp/src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2031/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/ti/ble/stack_util/config/build_components.opt" @"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/ti/ble/stack_util/config/factory_config.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -Oz -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_ble" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_ble/Release" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_ble/app" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/ti" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/ti/common/cc26xx" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/ti/posix/ticlang" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/third_party/freertos/include" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source/third_party/freertos/portable/GCC/ARM_CM0" -I"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/kernel/freertos" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_ble/sscp/inc" -DICALL_NO_APP_EVENTS -DCC23X0 -DNVOCMP_NWSAMEITEM=1 -DNVOCMP_NVPAGES=6 -DFREERTOS -DNVOCMP_POSIX_MUTEX -gdwarf-3 -Wunused-function -ffunction-sections -MMD -MP -MF"sscp/src/$(basename $(<F)).d_raw" -MT"$(@)" -I"F:/Projects/simple-system-control-protocol/simple-system-control-protocol/demos/lp_em_cc2340r5/sscp_ble/Release/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


