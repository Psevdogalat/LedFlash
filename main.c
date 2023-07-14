#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include <uart.h>
#include <shell.h>

#define LED1_PIN 3
#define LED2_PIN 4


/* settings */
char machineName[20];

/* registers */
uint16_t registers[20];
const size_t registersNumber = 20;

//=============================================================================

void initHardware()
{
	cli();

	DDRD = 0x1c;
	PORTD = 0x00;

	initUART();
	sei();
};

void initSettings()
{
	sprintf(machineName, "unit1");
};

int main(void)
{
	unsigned int leds;
	uint32_t counter;

	initHardware();	
	initSettings();
	initShell();

	leds = 0x01;
	counter = 0;


	while(1){
		if(counter == 0)
		{
			PORTD = leds << LED1_PIN;

			leds = leds << 1;	
			if(leds >= 0x04)
				leds = 0x01;

			counter = 0xFFFF;
			registers[1]++;
		}
		counter--;

		shell();
	};
};
