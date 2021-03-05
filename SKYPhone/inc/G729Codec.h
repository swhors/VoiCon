#pragma once

#include "va_g729a.h"

class CG729Codec
{
public:
	CG729Codec(void);
	~CG729Codec(void);
public:
	int Open(int direction);
	BYTE dataBuffer[1024];
    int bufferSize;
	int Encode( BYTE *buffer, int size );
    BOOL Decode( BYTE *buffer, int length, int *retlen );
	BOOL Decode( BYTE *buffer, int length, BYTE *outbuf, int *retlen );
};
