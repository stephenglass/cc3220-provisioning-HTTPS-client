################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/sglas/wifiMonitorCC3220/solarMonitor_CC3220S_LAUNCHXL_freertos_ccs" --include_path="C:/ti/simplelink_cc32xx_sdk_2_40_02_00/source" --include_path="C:/ti/simplelink_cc32xx_sdk_2_40_02_00/source/ti/posix/ccs" --include_path="C:/ti/FreeRTOSv10.0.0/FreeRTOS/Source/include" --include_path="C:/ti/FreeRTOSv10.0.0/FreeRTOS/Source/portable/CCS/ARM_CM3" --include_path="C:/Users/sglas/wifiMonitorCC3220/freertos_builds_CC3220S_LAUNCHXL_release_ccs" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/include" -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


