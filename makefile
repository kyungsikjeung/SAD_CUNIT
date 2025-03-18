	################################################################################
	# Automatically-generated file. Do not edit!
	################################################################################

	-include ../makefile.init

	RM := rm -rf

	# All of the sources participating in the build are defined here
	-include sources.mk
	-include utilities/subdir.mk
	-include usb/host/class/subdir.mk
	-include usb/host/subdir.mk
	-include startup/subdir.mk
	-include sources/userdrivers/subdir.mk
	-include sources/subdir.mk
	-include source/subdir.mk
	-include osa/subdir.mk
	-include fatfs/src/subdir.mk
	-include fatfs/subdir.mk
	-include drivers/subdir.mk
	-include board/subdir.mk
	-include CMSIS/subdir.mk
	ifneq ($(MAKECMDGOALS),clean)
	ifneq ($(strip $(C_DEPS)),)
	-include $(C_DEPS)
	endif
	endif

	-include ../makefile.defs

	OPTIONAL_TOOL_DEPS := \
	$(wildcard ../makefile.defs) \
	$(wildcard ../makefile.init) \
	$(wildcard ../makefile.targets) \


	BUILD_ARTIFACT_NAME := MK60D10_InterfaceBoard
	BUILD_ARTIFACT_EXTENSION := axf
	BUILD_ARTIFACT_PREFIX :=
	BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

	# Add inputs and outputs from these tool invocations to the build variables 

	# All Target
	all:
		+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

	# Main-build Target
	main-build: MK60D10_InterfaceBoard.axf

	# Tool invocations
	MK60D10_InterfaceBoard.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
		@echo 'Building target: $@'
		@echo 'Invoking: MCU Linker'
		arm-none-eabi-gcc -nostdlib -Xlinker -Map="MK60D10_InterfaceBoard.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m4 -mthumb -T MK60D10_InterfaceBoard_Debug.ld -o "MK60D10_InterfaceBoard.axf" $(OBJS) $(USER_OBJS) $(LIBS)
		@echo 'Finished building target: $@'
		@echo ' '

	# Other Targets
	clean:
		-$(RM) MK60D10_InterfaceBoard.axf
		-@echo ' '

	post-build:
		-@echo 'Performing post-build steps'
		-arm-none-eabi-size "MK60D10_InterfaceBoard.axf"; # arm-none-eabi-objcopy -v -O binary "MK60D10_InterfaceBoard.axf" "MK60D10_InterfaceBoard.bin" ; # checksum -p MK60DX256xxx10 -d "MK60D10_InterfaceBoard.bin";
		-@echo ' '

	.PHONY: all clean dependents main-build post-build

	-include ../makefile.targets
