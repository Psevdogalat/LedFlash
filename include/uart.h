#ifndef __UART_H__
#define __UART_H__

#include <stdlib.h>

#ifndef UART_BaudRate
	#define UART_BaudRate 9600UL
#endif

#define UART_UBRR (F_CPU/(16 * UART_BaudRate) + 1)
#define UART_RecvBufferSize	256 
#define UART_SendBufferSize	512 

void initUART();
size_t UARTSend(const char *, size_t );
size_t UARTRecv(char *, size_t );
void UARTEcho(char );

#endif
