.PHONY: all clean terminal test

TTYS=/dev/ttyS2
BAUDRATE=9600
MCU=atmega328p
DUDE_PROGER=usbasp
DUDE_MCU=m328p

CLOCK=8000000

CC=avr-gcc
CXX_FLG=-mmcu=$(MCU) -DF_CPU=$(CLOCK)UL -Os -Wall -fmax-errors=5 -std=c99 \
	-DUART_BaudRate=$(BAUDRATE)UL -Iinclude

MAINTARGET=firmware.hex
TARGETFIRMWARE=$(MAINTARGET)

MODULES_SRC=$(wildcard *.c)
MODULES=$(MODULES_SRC:.c=.o)

all: $(MAINTARGET)

.c.o:
	$(CC) $< $(CXX_FLG) -c -o $@

firmware.o:	$(MODULES)
	$(CC) $^ $(CXX_FLG) -o $@

clean:
	$(RM) *.o
	$(RM) *.hex

firmware.hex: firmware.o
	$(RM) $@
	avr-size --format=avr --mcu=$(MCU) $<
	avr-objcopy -j .text -j .data -O ihex $< $@

flash: $(TARGETFIRMWARE)
	avrdude -p  $(DUDE_MCU) -c $(DUDE_PROGER) -U flash:w:$<:i

terminal:
	tio -p odd -b $(BAUDRATE) $(TTYS)
	
	
	
