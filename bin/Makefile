###############################################################################
# Makefile for the project SAE_AutoShifter
###############################################################################

## General Flags
PROJECT = SAE_AutoShifter
MCU = atmega328p
TARGET = SAE_AutoShifter.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99   -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=SAE_AutoShifter.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## avrdude flags
AVRDUDE_FLAGS = -p m328p -c arduino -P com5 -b 57600


## Include Directories
INCLUDES = -I"./" -I"./src" 

## Objects that must be built in order to link
OBJECTS = delay_rg.o main.o SAE_AutoShifter.o serial.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) SAE_AutoShifter.hex SAE_AutoShifter.eep SAE_AutoShifter.lss size

## Compile
delay_rg.o: ../src/delay_rg.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

SAE_AutoShifter.o: ../src/SAE_AutoShifter.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

serial.o: ../src/serial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Link
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

## Upload
bupload: all upload

upload:
	avrdude $(AVRDUDE_FLAGS) -F -U flash:w:${PROJECT}.hex

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) SAE_AutoShifter.elf dep/* SAE_AutoShifter.hex SAE_AutoShifter.eep SAE_AutoShifter.lss SAE_AutoShifter.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

