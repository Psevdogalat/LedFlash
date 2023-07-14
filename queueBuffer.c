#include <queueBuffer.h>
#include <string.h>

#define _buffer this->buffer
#define _size this->size
#define _fill this->fill
#define _readPos this->readPos
#define _writePos this->writePos

void initQueueBuffer(struct QueueBuffer * this, char * buffer, 
	size_t size)
{
	_buffer = buffer;
	_size = size;
	_fill = 0;
	_readPos = 0;
	_writePos = 0;
};

void pushQueueBuffer(struct QueueBuffer * this, char inVal)
{
	if(_fill < _size)
	{
		_buffer[_writePos++] = inVal;
		if(_writePos >= _size) _writePos = 0;
		_fill++;
	}

};

char popQueueBuffer(struct QueueBuffer * this)
{
	char retVal;
	if(_fill)
	{
		retVal = _buffer[_readPos++];
		if(_readPos >= _size) _readPos = 0;
		_fill--;
	}

	return retVal;
};

size_t readQueueBuffer(struct QueueBuffer * this, char * dstBuffer,
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
	const char * srcBuffer, const size_t dataSize)
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

