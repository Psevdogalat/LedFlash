#include <terminal.h>
#include <uart.h>
#include <string.h>


void printTerminal(const char * string)
{
	UARTSend(string, strlen(string));
};

void lfTerminal()
{
	char buffer[2];
	buffer[0] = '\r';
	buffer[1] = '\n';
	UARTSend(buffer, 2);
};

void termSGR_reset()
{
	char buffer[3];
	buffer[0] = ASCII_ESC;
	buffer[1] = '[';
	buffer[2] = 'm';
	UARTSend(buffer, 3);
};

void termSGR_fGreen()
{
	char buffer[5];
	buffer[0] = ASCII_ESC;
	buffer[1] = '[';
	buffer[2] = '3';
	buffer[3] = '2';
	buffer[4] = 'm';
	UARTSend(buffer, 5);
};

