################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../arduinolib/WiFi/WiFi.cpp \
../arduinolib/WiFi/WiFiClient.cpp \
../arduinolib/WiFi/WiFiServer.cpp \
../arduinolib/WiFi/WiFiUdp.cpp 

OBJS += \
./arduinolib/WiFi/WiFi.o \
./arduinolib/WiFi/WiFiClient.o \
./arduinolib/WiFi/WiFiServer.o \
./arduinolib/WiFi/WiFiUdp.o 

CPP_DEPS += \
./arduinolib/WiFi/WiFi.d \
./arduinolib/WiFi/WiFiClient.d \
./arduinolib/WiFi/WiFiServer.d \
./arduinolib/WiFi/WiFiUdp.d 


# Each subdirectory must supply rules for building sources it contributes
arduinolib/WiFi/%.o: ../arduinolib/WiFi/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\Arduino\workspace\arduino_core\src" -I"C:\Arduino\workspace\PIR-Autocamera\arduinolib" -I"C:\Arduino\workspace\PIR-Autocamera\lib" -DARDUINO=22 -Wall -g2 -gstabs -Os -ffunction-sections -fdata-sections -ffunction-sections -fdata-sections -fno-exceptions -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


