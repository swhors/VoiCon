#include "stdafx.h"
#include "g729codec.h"

CG729Codec::CG729Codec(void)
{
}

CG729Codec::~CG729Codec(void)
{
}

int CG729Codec::Open(int direction)
{
	if( direction == 0 )
	{
		// decoding
		va_g729a_init_decoder();
	}
	else
	{
		// encoding
		va_g729a_init_encoder();
	}
	return 1;
}

int CG729Codec::Encode( BYTE *buffer, int size )
{
	memset(dataBuffer, 0, 1024);
	va_g729a_encoder((short *)buffer, (unsigned char *)&dataBuffer[0]);
	return 1;
}
BOOL CG729Codec::Decode( BYTE *buffer, int length, int *retlen )
{
	int bfi = 0;
	memset(dataBuffer, 0, 1024);
	//assume bfi=0
	if( buffer )
        va_g729a_decoder(buffer, (short*)&dataBuffer[0], bfi);
    return TRUE;
}

BOOL CG729Codec::Decode( BYTE *buffer, int length, BYTE *outbuf, int *retlen )
{
	int bfi = 0;
	//assume bfi=0
	memset(outbuf, 0, *retlen);
	if( buffer )
    va_g729a_decoder(buffer, (short*)outbuf, bfi);
    return TRUE;
}