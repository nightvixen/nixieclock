###############################################################################
# Makefile for the project NixieClockFW
###############################################################################

## General Flags
PROJECT = NixieClockFW
MCU = atmega8
TARGET = NixieClockFW.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99               -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=NixieClockFW.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o pca2129_i2c.o twimaster.o buzzer.o melody.o eeprom.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) NixieClockFW.hex NixieClockFW.eep NixieClockFW.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pca2129_i2c.o: ../I2C/pca2129_i2c.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

twimaster.o: ../twimaster.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

buzzer.o: ../buzzer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

melody.o: ../melody.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

eeprom.o: ../eeprom.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) NixieClockFW.elf dep/* NixieClockFW.hex NixieClockFW.eep NixieClockFW.lss NixieClockFW.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

