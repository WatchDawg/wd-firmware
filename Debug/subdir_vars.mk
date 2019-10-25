################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blink.cpp 

CMD_SRCS += \
../lnk_msp430fr2476.cmd 

LIB_SRCS += \
../IQmathLib_CCS_MPYsoftware_CPUX_large_code_small_data.lib \
../QmathLib_CCS_MPYsoftware_CPUX_large_code_small_data.lib 

C_SRCS += \
../magnetometer.c 

C_DEPS += \
./magnetometer.d 

OBJS += \
./blink.obj \
./magnetometer.obj 

CPP_DEPS += \
./blink.d 

OBJS__QUOTED += \
"blink.obj" \
"magnetometer.obj" 

C_DEPS__QUOTED += \
"magnetometer.d" 

CPP_DEPS__QUOTED += \
"blink.d" 

CPP_SRCS__QUOTED += \
"../blink.cpp" 

C_SRCS__QUOTED += \
"../magnetometer.c" 


