################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../util.cc 

CPP_SRCS += \
../MCMF.cpp \
../cdn.cpp \
../deploy.cpp \
../graph.cpp \
../io.cpp 

CC_DEPS += \
./util.d 

OBJS += \
./MCMF.o \
./cdn.o \
./deploy.o \
./graph.o \
./io.o \
./util.o 

CPP_DEPS += \
./MCMF.d \
./cdn.d \
./deploy.d \
./graph.d \
./io.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/full/Desktop/teambuilder-322/cdn/lib -O2 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/full/Desktop/teambuilder-322/cdn/lib -O2 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


