################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../28002x_generic_ram_lnk.cmd 

SYSCFG_SRCS += \
../i2c_ex1_loopback.syscfg 

LIB_SRCS += \
C:/ti/C2000Ware_4_00_00_00/driverlib/f28002x/driverlib/ccs/Debug/driverlib.lib 

C_SRCS += \
../i2c_ex1_loopback.c \
./syscfg/board.c 

GEN_FILES += \
./syscfg/board.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./i2c_ex1_loopback.d \
./syscfg/board.d 

OBJS += \
./i2c_ex1_loopback.obj \
./syscfg/board.obj 

GEN_MISC_FILES += \
./syscfg/board.h \
./syscfg/pinmux.csv 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"i2c_ex1_loopback.obj" \
"syscfg\board.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\board.h" \
"syscfg\pinmux.csv" 

C_DEPS__QUOTED += \
"i2c_ex1_loopback.d" \
"syscfg\board.d" 

GEN_FILES__QUOTED += \
"syscfg\board.c" 

C_SRCS__QUOTED += \
"../i2c_ex1_loopback.c" \
"./syscfg/board.c" 

SYSCFG_SRCS__QUOTED += \
"../i2c_ex1_loopback.syscfg" 


