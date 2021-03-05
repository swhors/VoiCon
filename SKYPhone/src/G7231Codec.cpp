// G7231Codec.cpp: implementation of the CG7231Codec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ubidialer.h"
#include "G7231Codec.h"
#include "WaveDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CG7231Codec::CG7231Codec()
{

}

CG7231Codec::~CG7231Codec()
{

}

int CG7231Codec::Open(int bEncOpen)
{
	if( bEncOpen == 1 )
	{
		this->dwOutType = WAVE_FORMAT_ON_G723;
		this->dwInType = WAVE_FORMAT_PCM;
		return CACMCodec::Open(1);
	}
	else if( bEncOpen == 0 )
	{
		this->dwInType = WAVE_FORMAT_ON_G723;
		this->dwOutType = WAVE_FORMAT_PCM;
		return CACMCodec::Open(0);
	}
	return 0;
}

int CG7231Codec::Encode(PBYTE buffer, int size)
{
	return CACMCodec::Encode(buffer, size);
}

BOOL CG7231Codec::Decode(PBYTE pBuf, int length, PBYTE pOutBuf)
{
	//if( CACMCodec::Open(bEncOpen) )
	return FALSE;
}

void CG7231Codec::Close()
{
	CACMCodec::Close();
}

int CG7231Codec::Decode(PBYTE pBuf, int dLen, int *retLen)
{
	return CACMCodec::Decode(pBuf, dLen, retLen);
}
