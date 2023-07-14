#ifndef __FORMAT_H__
#define __FORMAT_H__

	#include <stdlib.h>
	#include <stdint.h>
	
	size_t pHEXUInt8_t(char * buffer, size_t bufferSize, uint8_t value);
	size_t pHEXUInt16_t(char * buffer, size_t bufferSize, uint16_t value);
	size_t pHEXUInt32_t(char * buffer, size_t bufferSize, uint32_t value);
	size_t pDECUInt8_t(char * buffer, size_t bufferSize, uint8_t value);
	size_t pDECUInt16_t(char * buffer, size_t bufferSize, uint16_t value);

#endif
