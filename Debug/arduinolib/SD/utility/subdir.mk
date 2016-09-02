################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../arduinolib/SD/utility/Sd2Card.cpp \
../arduinolib/SD/utility/SdFile.cpp \
../arduinolib/SD/utility/SdVolume.cpp 

OBJS += \
./arduinolib/SD/utility/Sd2Card.o \
./arduinolib/SD/utility/SdFile.o \
./arduinolib/SD/utility/SdVolume.o 

CPP_DEPS += \
./arduinolib/SD/utility/Sd2Card.d \
./arduinolib/SD/utility/SdFile.d \
./arduinolib/SD/utility/SdVolume.d 


# Each subdirectory must supply rules for building sources it contributes
arduinolib/SD/utility/%.o: ../arduinolib/SD/utility/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\Arduino\workspace\arduino_core\src" -I"C:\Arduino\workspace\PIR-Autocamera\arduinolib" -I"C:\Arduino\workspace\PIR-Autocamera\lib" -DARDUINO=22 -Wall -g2 -gstabs -Os -ffunction-sections -fdata-sections -ffunction-sections -fdata-sections -fno-exceptions -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


