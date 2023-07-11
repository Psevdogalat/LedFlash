#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>

#define LED1_PIN 3
#define LED2_PIN 4


#define UART_BaudRate 9600UL
#define UART_UBRR (F_CPU/(16 * UART_BaudRate) + 1)

#define UART_RecvBufferSize 256
#define UART_SendBufferSize 256

struct QueueBuffer
{
	unsigned char * buffer;
	size_t size;
	size_t fill;
	size_t readPos;
	size_t writePos;
};


#define _buffer this->buffer
#define _size this->size
#define _fill this->fill
#define _readPos this->readPos
#define _writePos this->writePos

void initQueueBuffer(struct QueueBuffer * this, unsigned char * buffer, 
	size_t size)
{
	_buffer = buffer;
	_size = size;
	_fill = 0;
	_readPos = 0;
	_writePos = 0;
};

void pushQueueBuffer(struct QueueBuffer * this, unsigned char inVal)
{
	if(_fill < _size)
	{
		_buffer[_writePos++] = inVal;
		if(_writePos >= _size) _writePos = 0;
		_fill++;
	}

};

unsigned char popQueueBuffer(struct QueueBuffer * this)
{
	unsigned char retVal;
	if(_fill)
	{
		retVal = _buffer[_readPos++];
		if(_readPos >= _size) _readPos = 0;
		_fill--;
	}

	return retVal;
};

size_t readQueueBuffer(struct QueueBuffer * this, unsigned char * dstBuffer,
	const size_t dataSize)
{
	size_t dataRead;
	size_t section1;
	size_t section2;

	if(_fill == 0) return 0;
	dataRead = (_fill < dataSize)? _fill: dataSize;
	section1 = _size - _readPos;
	if(_readPos < _writePos || section1 > dataRead)
	{
		memcpy(dstBuffer, _buffer + _readPos, dataRead);
		_readPos += dataRead;
	}
	else
	{
		section2 = dataRead - section1;
		memcpy(dstBuffer, _buffer + _readPos, section1);
		memcpy(dstBuffer + section1, _buffer, section2);
		_readPos = section2;
	};

	_fill -= dataRead;
	return dataRead;
};

size_t writeQueueBuffer(struct QueueBuffer * this, 
	const unsigned char * srcBuffer, const size_t dataSize)
{
	size_t dataWrite;
	size_t freeSpace;
	size_t section1;
	size_t section2;

	freeSpace = _size - _fill;
	if(freeSpace == 0) return 0;
	dataWrite = (freeSpace < dataSize)? freeSpace: dataSize;
	section1 = _size - _writePos;
	if(_writePos < _readPos || section1 > dataWrite)
	{
		memcpy(_buffer + _writePos, srcBuffer, dataWrite);
		_writePos += dataWrite;
	}
	else
	{
		section2 = dataWrite - section1;
		memcpy(_buffer + _writePos, srcBuffer, section1);
		memcpy(_buffer, srcBuffer + section1, section2);
		_writePos = section2;
	};

	_fill += dataWrite;
	return dataWrite;

};


#undef _buffer
#undef _size
#undef _fill
#undef _readPos
#undef _writePos

static unsigned char _RecvBufferMem[UART_RecvBufferSize];
static unsigned char _SendBufferMem[UART_SendBufferSize];
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

	UBRR0L = (unsigned char)(UART_UBRR );
	UBRR0H = (unsigned char)(UART_UBRR >> 8);
	UCSR0A = 0;
	UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) |
		(1 << TXEN0);
	UCSR0C = (1 << UPM01) | (1 << UPM00) | (1 << UCSZ01) |
		(1 << UCSZ00);	
};

size_t UARTSend(const unsigned char * buffer, size_t dataSize)
{
	size_t dataWrite;
	cli();

	dataWrite = writeQueueBuffer(&UART_SendBuffer, buffer, dataSize);

	if( UCSR0A & (1 << UDRE0) && UART_SendBuffer.fill)	
		UDR0 = popQueueBuffer(&UART_SendBuffer);

	sei();

	return dataWrite;
};

size_t UARTRecv(unsigned char * buffer, size_t dataSize)
{
	size_t dataRead;
	cli();
	dataRead = readQueueBuffer(&UART_RecvBuffer, buffer, dataSize);
	sei();

	return dataRead;
};

void UARTEcho(unsigned char value)
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


unsigned char machineName[20];

int main(void)
{
	unsigned int leds;
	unsigned char buffer[256];
	unsigned char cmdLine[256];
	size_t cmdLineLength = 0;

	memcpy(machineName, "unit1", 5);
	machineName[5] ='\0';
	
	cli();

	DDRD = 0x1c;
	leds = 0x01;

	PORTD = 0x00;

	initUART();
	sei();

	size_t dataRecv;
	size_t dataSend;

	dataRecv = 0;
	dataSend= 0;


	bool isNormalChar(unsigned char value)
	{
		if(value >= 0x20 && value <= 0x7E)
			return true;

		return false; 
	};

	#define ASCII_CR 0x0D
	#define ASCII_LF 0x0A
	#define ASCII_ESC 0x1B
	#define ASCII_BS 0x08
	#define ASCII_DEL 0x7F
	#define ASCII_ETX 0x03

	void termSGR_reset()
	{
		unsigned char buffer[20];
		buffer[0] = ASCII_ESC;
		buffer[1] = '[';
		memcpy(buffer + 2, "0m", 2);
		UARTSend(buffer, 4);
	};

	void termSGR_fGreen()
	{
		unsigned char buffer[20];
		buffer[0] = ASCII_ESC;
		buffer[1] = '[';
		memcpy(buffer + 2, "32m", 3);
		UARTSend(buffer, 5);
	};
	
	void echoCmdLinePrompt()
	{
		unsigned char buffer[256];

		size_t offset = 0;
		size_t lineSize = 0;

		lineSize = strlen(machineName);
		memcpy(buffer, machineName, lineSize);
		offset += lineSize;

		memcpy(buffer + offset, "# ", 2);
		offset += 2;	 
		
		termSGR_fGreen();
		UARTSend(buffer, offset);
		termSGR_reset();	

	};

	void processCmdLine()
	{

	};

	while(1){

		PORTD = leds << LED1_PIN;
		_delay_ms(10);

		leds = leds << 1;	
		if(leds >= 0x04)
			leds = 0x01;
	
		dataRecv = UARTRecv(buffer, 256);
		unsigned int i;
		for(i = 0; i < dataRecv; i++)
		{

			#define _curChar buffer[i] 

			if(isNormalChar(_curChar))
			{
				cmdLine[cmdLineLength++] = _curChar;
				UARTEcho(_curChar);
			}
			else
			{
				if(_curChar == ASCII_CR)
				{
					UARTEcho(ASCII_CR);
					UARTEcho(ASCII_LF);
					processCmdLine();
					cmdLineLength = 0;
					echoCmdLinePrompt();
				}
				else 
				if(_curChar == ASCII_BS || _curChar == ASCII_DEL)
				{
					if(cmdLineLength)
					{
						UARTEcho(ASCII_BS);
						UARTEcho(' ');
						UARTEcho(ASCII_BS);
						cmdLineLength--;
					}
				}
				else
				if(_curChar == ASCII_ETX)
				{
					UARTSend("\r\n", 2);
					UARTSend("break command\r\n", 15);		
					cmdLineLength = 0;
					echoCmdLinePrompt();
				};
			};
		};
	};
};
