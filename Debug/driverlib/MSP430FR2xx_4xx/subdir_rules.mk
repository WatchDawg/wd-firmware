################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
driverlib/MSP430FR2xx_4xx/%.obj: ../driverlib/MSP430FR2xx_4xx/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/patrick/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/patrick/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg/Ublox_GPS" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg/magnetometer" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg/common" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg/epaper" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg/FreeRTOS/include" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg/driverlib/MSP430FR2xx_4xx" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg/FreeRTOS/portable" --include_path="/mnt/WIN_C/Users/Patrick Chong/Documents/dev/Watchdawg" --include_path="/home/patrick/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2476__ --define=_FRWP_ENABLE --define=_INFO_FRWP_ENABLE -g --printf_support=nofloat --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="driverlib/MSP430FR2xx_4xx/$(basename $(<F)).d_raw" --obj_directory="driverlib/MSP430FR2xx_4xx" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


