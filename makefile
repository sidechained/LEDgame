TARGET = faust2stm
ARM_TOOLCHAIN_PATH=/Applications/ArmGNUToolchain/11.3.rel1/arm-none-eabi/bin
STM_STDPERIPH_PATH=stm/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0
STM_DISCOVERY_PATH=stm/STM32F4-Discovery_FW_V1.1.0
DRIVERS_PATH=drivers

SRCS=faust2stm.cc $(DRIVERS_PATH)/audio_dac.cc $(DRIVERS_PATH)/leds.cc

# Discovery board firmware source files:
SRCS+=$(STM_DISCOVERY_PATH)/Utilities/STM32F4-Discovery/stm32f4_discovery.c
SRCS+=$(STM_DISCOVERY_PATH)/Project/Audio_playback_and_record/src/system_stm32f4xx.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
SRCS+=$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c

# 
SRCS+=$(STM_STDPERIPH_PATH)/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f40_41xxx.s

PP=$(ARM_TOOLCHAIN_PATH)/arm-none-eabi-g++
OBJCOPY=$(ARM_TOOLCHAIN_PATH)/arm-none-eabi-objcopy

CFLAGS = -g -O2 -Wall  # normally use -O2, but during OCD debug use -O0
CFLAGS = -Wno-register # suppress warning: ISO C++17 does not allow 'register' storage class specifier [-Wregister]
# Linker script for STM32F417IG Device with 1024KByte FLASH, 128KByte RAM (Q: why not for 407VG?)
CFLAGS += -T$(STM_STDPERIPH_PATH)/Project/STM32F4xx_StdPeriph_Templates/TrueSTUDIO/STM32F40_41xxx/STM32F417IG_FLASH.ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += --specs=nosys.specs
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -DSTM32F40_41xxx

CFLAGS += -I$(STM_DISCOVERY_PATH)/Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_DISCOVERY_PATH)/Project/Audio_playback_and_record/inc
CFLAGS += -I$(STM_DISCOVERY_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/inc

# "core_cm4.h": Cortex-M4 processor and core peripherals */
CFLAGS += -I$(STM_STDPERIPH_PATH)/Libraries/CMSIS/Include

# "stm32f4xx.h": CMSIS Peripheral Access Layer (register defs, bit defs, memory mapping)
CFLAGS += -I$(STM_STDPERIPH_PATH)/Libraries/CMSIS/Device/ST/STM32F4xx/Include

CFLAGS += -I/Applications/Faust-2.41.1/include/faust

.PHONY: $(TARGET)

all: $(TARGET)

$(TARGET): $(TARGET).elf

$(TARGET).elf: $(SRCS)
	$(PP) $(CFLAGS) $^ -o $@
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

flash-erase: $(TARGET)
	st-flash erase
	st-flash write $(TARGET).bin 0x8000000

flash-noerase: $(TARGET)
	st-flash write $(TARGET).bin 0x8000000

flash: $(TARGET)
	st-flash write $(TARGET).bin 0x8000000

clean:
	rm -f $(TARGET).elf $(TARGET).bin

# debugging on board (implement later)

# OPENOCD_BIN_PATH ?= /usr/local/bin/ # there
# OPENOCD_SCRIPT_PATH ?= /usr/local/share/openocd/scripts # there
# OPENOCD_INTERFACE_SCRIPT ?= /interface/stlink-v2.cfg # not there
# OPENOCD_TARGET_SCRIPT ?= /target/stm32f4x.cfg # not there

# debug_server: $(TARGET).bin
# 	$(OPENOCD_BIN_PATH)openocd \
# 		-s $(OPENOCD_SCRIPT_PATH) \
# 		-f $(OPENOCD_INTERFACE_SCRIPT) \
# 		-f $(OPENOCD_TARGET_SCRIPT) \
# 		-c "init;reset;halt" \

# debug_client:
# 	# DB MISSING
# 	$(DB) --eval-command="target remote localhost:3333" $(TARGET).elf