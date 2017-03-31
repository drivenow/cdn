################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../cdn/util.cc 

CPP_SRCS += \
../cdn/MCMF.cpp \
../cdn/cdn.cpp \
../cdn/deploy.cpp \
../cdn/io.cpp \
../cdn/server.cpp 

CC_DEPS += \
./cdn/util.d 

OBJS += \
./cdn/MCMF.o \
./cdn/cdn.o \
./cdn/deploy.o \
./cdn/io.o \
./cdn/server.o \
./cdn/util.o 

CPP_DEPS += \
./cdn/MCMF.d \
./cdn/cdn.d \
./cdn/deploy.d \
./cdn/io.d \
./cdn/server.d 


# Each subdirectory must supply rules for building sources it contributes
cdn/%.o: ../cdn/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/full/Desktop/teambuilder-322/cdn/lib" -O2 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cdn/%.o: ../cdn/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/full/Desktop/teambuilder-322/cdn/lib" -O2 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


