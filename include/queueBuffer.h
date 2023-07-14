#ifndef __QUEUEBUFFER_H__
#define __QUEUEBUFFER_H__

#include <stdlib.h>

struct QueueBuffer
{
	char * buffer;
	size_t size;
	size_t fill;
	size_t readPos;
	size_t writePos;
};

void initQueueBuffer(struct QueueBuffer *, char *, size_t );
void pushQueueBuffer(struct QueueBuffer *, char );
char popQueueBuffer(struct QueueBuffer * );
size_t readQueueBuffer(struct QueueBuffer *, char *, const size_t );
size_t writeQueueBuffer(struct QueueBuffer *, const char *, const size_t );

#endif
