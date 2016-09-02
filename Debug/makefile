################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include lib/subdir.mk
-include arduinolib/WiFi/utility/subdir.mk
-include arduinolib/WiFi/subdir.mk
-include arduinolib/Time/subdir.mk
-include arduinolib/SoftwareSerial/subdir.mk
-include arduinolib/SPI/subdir.mk
-include arduinolib/SD/utility/subdir.mk
-include arduinolib/SD/subdir.mk
-include arduinolib/Ethernet/utility/subdir.mk
-include arduinolib/Ethernet/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
PIR-Autocamera.lss \

FLASH_IMAGE += \
PIR-Autocamera.hex \

SIZEDUMMY += \
sizedummy \

AVRDUDEDUMMY += \
avrdudedummy \


# All Target
all: PIR-Autocamera.elf secondary-outputs

# Tool invocations
PIR-Autocamera.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-gcc --cref -s -Os -o"PIR-Autocamera.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lm -Wl,-Map,PIR-Autocamera.map,--cref -mrelax -Wl,--gc-sections -Wl,--gc-sections -L"C:\Arduino\workspace\arduino_core\2560_16MHz" -mmcu=atmega2560
	@echo 'Finished building target: $@'
	@echo ' '

PIR-Autocamera.lss: PIR-Autocamera.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S PIR-Autocamera.elf  >"PIR-Autocamera.lss"
	@echo 'Finished building: $@'
	@echo ' '

PIR-Autocamera.hex: PIR-Autocamera.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex PIR-Autocamera.elf  "PIR-Autocamera.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: PIR-Autocamera.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega2560 PIR-Autocamera.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: PIR-Autocamera.elf
	@echo 'Invoking: AVRDude'
	C:\WinAVR-20100110\bin\avrdude -pm2560 -carduino -P//./COM6 -b19200 -Uflash:w:PIR-Autocamera.hex:a
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(AVRDUDEDUMMY)$(S_DEPS)$(LSS)$(C_DEPS)$(FLASH_IMAGE)$(CC_DEPS)$(ELFS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(ASM_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(CPP_DEPS) PIR-Autocamera.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
