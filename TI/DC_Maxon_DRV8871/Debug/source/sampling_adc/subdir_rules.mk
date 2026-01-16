################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/sampling_adc/%.obj: ../source/sampling_adc/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1270/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R5 --code_state=32 --float_support=VFPv3D16 -me --include_path="C:/Users/mdilucchio/OneDrive - Fondazione Istituto Italiano Tecnologia/Desktop/DC_Maxon_DRV8871" --include_path="C:/Users/mdilucchio/OneDrive - Fondazione Istituto Italiano Tecnologia/Desktop/DC_Maxon_DRV8871/include" --include_path="C:/ti/ccs1270/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/sampling_adc/$(basename $(<F)).d_raw" --obj_directory="source/sampling_adc" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


