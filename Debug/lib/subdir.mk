################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/jsmn.cpp \
../lib/json_arduino.cpp 

OBJS += \
./lib/jsmn.o \
./lib/json_arduino.o 

CPP_DEPS += \
./lib/jsmn.d \
./lib/json_arduino.d 


# Each subdirectory must supply rules for building sources it contributes
lib/%.o: ../lib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\Arduino\workspace\arduino_core\src" -I"C:\Arduino\workspace\PIR-Autocamera\arduinolib" -I"C:\Arduino\workspace\PIR-Autocamera\lib" -DARDUINO=22 -Wall -g2 -gstabs -Os -ffunction-sections -fdata-sections -ffunction-sections -fdata-sections -fno-exceptions -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


