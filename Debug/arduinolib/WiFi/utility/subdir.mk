################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../arduinolib/WiFi/utility/server_drv.cpp \
../arduinolib/WiFi/utility/spi_drv.cpp \
../arduinolib/WiFi/utility/wifi_drv.cpp 

OBJS += \
./arduinolib/WiFi/utility/server_drv.o \
./arduinolib/WiFi/utility/spi_drv.o \
./arduinolib/WiFi/utility/wifi_drv.o 

CPP_DEPS += \
./arduinolib/WiFi/utility/server_drv.d \
./arduinolib/WiFi/utility/spi_drv.d \
./arduinolib/WiFi/utility/wifi_drv.d 


# Each subdirectory must supply rules for building sources it contributes
arduinolib/WiFi/utility/%.o: ../arduinolib/WiFi/utility/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\Arduino\workspace\arduino_core\src" -I"C:\Arduino\workspace\PIR-Autocamera\arduinolib" -I"C:\Arduino\workspace\PIR-Autocamera\lib" -DARDUINO=22 -Wall -g2 -gstabs -Os -ffunction-sections -fdata-sections -ffunction-sections -fdata-sections -fno-exceptions -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


