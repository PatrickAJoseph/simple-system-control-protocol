################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../sscp_serial.syscfg 

LDS_SRCS += \
../lpf3_cc23xx_freertos.lds 

C_SRCS += \
../main_freertos.c \
../sscp_serial.c \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c \
./syscfg/ti_freertos_config.c \
./syscfg/ti_freertos_portable_config.c 

GEN_FILES += \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c \
./syscfg/ti_utils_build_compiler.opt \
./syscfg/ti_freertos_config.c \
./syscfg/ti_freertos_portable_config.c 

GEN_MISC_DIRS += \
./syscfg 

C_DEPS += \
./main_freertos.d \
./sscp_serial.d \
./syscfg/ti_devices_config.d \
./syscfg/ti_drivers_config.d \
./syscfg/ti_freertos_config.d \
./syscfg/ti_freertos_portable_config.d 

GEN_OPTS += \
./syscfg/ti_utils_build_compiler.opt 

OBJS += \
./main_freertos.o \
./sscp_serial.o \
./syscfg/ti_devices_config.o \
./syscfg/ti_drivers_config.o \
./syscfg/ti_freertos_config.o \
./syscfg/ti_freertos_portable_config.o 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.genlibs \
./syscfg/ti_utils_build_linker.cmd.genmap \
./syscfg/syscfg_c.rov.xs \
./syscfg/FreeRTOSConfig.h 

GEN_MISC_DIRS__QUOTED += \
"syscfg" 

OBJS__QUOTED += \
"main_freertos.o" \
"sscp_serial.o" \
"syscfg\ti_devices_config.o" \
"syscfg\ti_drivers_config.o" \
"syscfg\ti_freertos_config.o" \
"syscfg\ti_freertos_portable_config.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\ti_utils_build_linker.cmd.genlibs" \
"syscfg\ti_utils_build_linker.cmd.genmap" \
"syscfg\syscfg_c.rov.xs" \
"syscfg\FreeRTOSConfig.h" 

C_DEPS__QUOTED += \
"main_freertos.d" \
"sscp_serial.d" \
"syscfg\ti_devices_config.d" \
"syscfg\ti_drivers_config.d" \
"syscfg\ti_freertos_config.d" \
"syscfg\ti_freertos_portable_config.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_devices_config.c" \
"syscfg\ti_drivers_config.c" \
"syscfg\ti_utils_build_compiler.opt" \
"syscfg\ti_freertos_config.c" \
"syscfg\ti_freertos_portable_config.c" 

C_SRCS__QUOTED += \
"../main_freertos.c" \
"../sscp_serial.c" \
"./syscfg/ti_devices_config.c" \
"./syscfg/ti_drivers_config.c" \
"./syscfg/ti_freertos_config.c" \
"./syscfg/ti_freertos_portable_config.c" 

SYSCFG_SRCS__QUOTED += \
"../sscp_serial.syscfg" 


