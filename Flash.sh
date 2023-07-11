#!/bin/sh
PARTNO=m328p
PROGRAMMER=arduino  

avrdude -v -p $PARTNO -c $PROGRAMMER -P $1 -U flash:w:firmware.hex 