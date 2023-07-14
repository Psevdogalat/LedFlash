#include <format.h>

static const char hexMap[] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f' 
};

size_t pHEXUInt8_t(char * buffer, size_t bufferSize, uint8_t value)
{
	unsigned int i, wide;
	uint8_t mask;

	for( wide = 2, mask = 0xF0; wide > 1; wide--, mask >>= 4)
		if( value & mask )
			break; 

	wide = (wide < bufferSize)? wide : bufferSize;	
	for(i = wide; i--; value >>= 4)
		buffer[i] = hexMap[value & 0x0F];	
	
	return wide;
};

size_t pHEXUInt16_t(char * buffer, size_t bufferSize, uint16_t value)
{
	unsigned int i, wide;
	uint16_t mask;

	for( wide = 4, mask = 0xF000; wide > 1; wide--, mask >>= 4)
		if( value & mask )
			break; 

	wide = (wide < bufferSize)? wide : bufferSize;	
	for(i = wide; i--; value >>= 4)
		buffer[i] = hexMap[value & 0x0F];	
	
	return wide;
};

size_t pHEXUInt32_t(char * buffer, size_t bufferSize, uint32_t value)
{
	unsigned int i, wide;
	uint32_t mask;

	for( wide = 8, mask = 0xF0000000; wide > 1; wide--, mask >>= 4)
		if( value & mask )
			break; 

	wide = (wide < bufferSize)? wide : bufferSize;	
	for(i = wide; i--; value >>= 4)
		buffer[i] = hexMap[value & 0x0F];	
	
	return wide;
};

size_t pDECUInt8_t(char * buffer, size_t bufferSize, uint8_t value)
{
	unsigned int i,c;
	uint16_t bcd;

	bcd = 0;
	for(c = 8; c--; )
	{
		for( i = 16; i; )
		{
			i -= 4;
			if( ((bcd >> i) & 0x000F) >= 5 )
				bcd += (3 << i);
		}

		bcd <<= 1;
		if( value & 0x80 ) 
			bcd++;

		value <<= 1;
		
	}
	
	return pHEXUInt16_t(buffer, bufferSize, bcd);
};

size_t pDECUInt16_t(char * buffer, size_t bufferSize, uint16_t value)
{
	unsigned int i,c;
	uint32_t bcd;

	bcd = 0;
	for(c = 16; c--; )
	{
		for( i = 32; i; )
		{
			i -= 4;
			if( ((bcd >> i) & 0x0000000F) >= 5 )
				bcd += (3 << i);
		}

		bcd <<= 1;
		if( value & 0x8000 ) 
			bcd++;

		value <<= 1;
		
	}

	return pHEXUInt32_t(buffer, bufferSize, bcd);
};
