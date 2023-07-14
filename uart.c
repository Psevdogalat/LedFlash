#include <uart.h>
#include <queueBuffer.h>
#include <avr/interrupt.h>
#include <avr/io.h>

static char _RecvBufferMem[UART_RecvBufferSize];
static char _SendBufferMem[UART_SendBufferSize];
static struct QueueBuffer UART_RecvBuffer;
static struct QueueBuffer UART_SendBuffer;

ISR(USART_RX_vect)
{
	cli();
	if(UART_RecvBuffer.fill < UART_RecvBuffer.size)
		pushQueueBuffer(&UART_RecvBuffer, UDR0);

	sei();
};

ISR(USART_TX_vect)
{
	cli();
	if(UART_SendBuffer.fill)
		UDR0 = popQueueBuffer(&UART_SendBuffer);
	//else
	//	UCSR0B &= ~(1 << TXEN0);	

	sei();
};

void initUART()
{
	initQueueBuffer(&UART_SendBuffer, _SendBufferMem, UART_SendBufferSize); 
	initQueueBuffer(&UART_RecvBuffer, _RecvBufferMem, UART_RecvBufferSize); 

	UBRR0L = (char)(UART_UBRR );
	UBRR0H = (char)(UART_UBRR >> 8);
	UCSR0A = 0;
	UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) |
		(1 << TXEN0);
	UCSR0C = (1 << UPM01) | (1 << UPM00) | (1 << UCSZ01) |
		(1 << UCSZ00);	
};

size_t UARTSend(const char * buffer, size_t dataSize)
{
	size_t dataWrite;
	cli();

	dataWrite = writeQueueBuffer(&UART_SendBuffer, buffer, dataSize);

	if( UCSR0A & (1 << UDRE0) && UART_SendBuffer.fill)	
		UDR0 = popQueueBuffer(&UART_SendBuffer);

	sei();

	return dataWrite;
};

size_t UARTRecv(char * buffer, size_t dataSize)
{
	size_t dataRead;
	cli();
	dataRead = readQueueBuffer(&UART_RecvBuffer, buffer, dataSize);
	sei();

	return dataRead;
};

void UARTEcho(char value)
{
	cli();
	
	if( UCSR0A & (1 << UDRE0))	
	{
		if(UART_SendBuffer.fill)
		{
			pushQueueBuffer(&UART_SendBuffer, value);
			UDR0 = popQueueBuffer(&UART_SendBuffer);
		}
		else
			UDR0 = value;
	}
	else
		pushQueueBuffer(&UART_SendBuffer, value);


	sei();
};

