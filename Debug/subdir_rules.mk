################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/evan/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/evan/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/home/evan/CCS/workspace_v9/Blink/FreeRTOS/include" --include_path="/home/evan/CCS/workspace_v9/Blink/driverlib/MSP430FR2xx_4xx" --include_path="/home/evan/CCS/workspace_v9/Blink/FreeRTOS/portable" --include_path="/home/evan/CCS/workspace_v9/Blink" --include_path="/home/evan/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2476__ --define=_FRWP_ENABLE --define=_INFO_FRWP_ENABLE -g --printf_support=nofloat --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


