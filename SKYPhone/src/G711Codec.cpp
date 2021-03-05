#include "stdafx.h"
#include "g711codec.h"

CG711Codec::CG711Codec(void)
{
	ulaw = 1;
}

CG711Codec::~CG711Codec(void)
{
}

int CG711Codec::Encode( BYTE *buffer, int size )
{
	memset(dataBuffer, 0, 512);
	if( !ulaw )
	{
		return ALawEncode(dataBuffer,(int16*)buffer, size);
	}
	return ULawEncode(dataBuffer,(int16*)buffer, size);
}

int CG711Codec::Decode( BYTE *buffer, int size )
{
	memset(dataBuffer, 0, 512);
	if( !ulaw )
	{
		return ALawDecode((int16 *)&dataBuffer[0],(uint8*)buffer, size);
	}
	return ULawDecode((int16 *)&dataBuffer[0],(uint8*)buffer, size);
}
