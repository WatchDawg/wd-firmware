################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/IAR/RX600/%.obj: ../FreeRTOS/portable/IAR/RX600/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/evan/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/evan/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/home/evan/CCS/workspace_v9/Blink/FreeRTOS/include" --include_path="/home/evan/CCS/workspace_v9/Blink" --include_path="/home/evan/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2476__ --define=_FRWP_ENABLE --define=_INFO_FRWP_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="FreeRTOS/portable/IAR/RX600/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS/portable/IAR/RX600" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS/portable/IAR/RX600/%.obj: ../FreeRTOS/portable/IAR/RX600/%.s $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/evan/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/evan/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/home/evan/CCS/workspace_v9/Blink/FreeRTOS/include" --include_path="/home/evan/CCS/workspace_v9/Blink" --include_path="/home/evan/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2476__ --define=_FRWP_ENABLE --define=_INFO_FRWP_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="FreeRTOS/portable/IAR/RX600/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS/portable/IAR/RX600" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


