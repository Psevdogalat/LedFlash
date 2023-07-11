#!/bin/sh	
MCU=atmega328p
CLOCK=8000000UL
FLAGS= -Os -Wall -fmax-errors=5

avr-gcc -mmcu=$MCU -DF_CPU=$CLOCK $FLAGS -c -o sub_main.o main.c
[ $? -ne 0 ] && exit 1

avr-gcc -mmcu=$MCU -DF_CPU=$CLOCK $FLAGS -o main.o sub_main.o
[ $? -ne 0 ] && exit 1

rm -f firmware.hex
avr-objcopy -j .text -j .data -O ihex  main.o firmware.hex
avr-size --format=avr --mcu=$MCU main.o
avrdude.exe -p m328p -c usbasp -U flash:w:firmware.hex:i
tio -b 9600 -p odd /dev/ttyS11
