#pragma once
#include "g711.h"

class CG711Codec :
	public G711
{
public:
	int ulaw; // default 0 = mlaw
	BYTE dataBuffer[512];
    int bufferSize;
	CG711Codec(void);
	~CG711Codec(void);
	void ulaw_enable(){ulaw = 1;};
	void ulaw_disable(){ulaw = 0;};
    int Encode( BYTE *buffer, int size );
	int Decode( BYTE *buffer, int size );
};
