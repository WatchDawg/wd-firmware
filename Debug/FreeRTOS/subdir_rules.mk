################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/%.obj: ../FreeRTOS/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476/Ublox_GPS" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476/magnetometer" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476/common" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476/epaper" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476/FreeRTOS/include" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476/driverlib/MSP430FR2xx_4xx" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476/FreeRTOS/portable" --include_path="C:/Users/Amir4/OneDrive/Documents/School/Umich Senior/EECS-473/blink_freertos_msp430fr2476" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2476__ --define=_FRWP_ENABLE --define=_INFO_FRWP_ENABLE -g --printf_support=nofloat --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="FreeRTOS/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


