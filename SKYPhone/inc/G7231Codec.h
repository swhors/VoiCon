// G7231Codec.h: interface for the CG7231Codec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_G7231CODEC_H__FA593C31_8EDB_4CBF_8E59_BED7FE26D6A4__INCLUDED_)
#define AFX_G7231CODEC_H__FA593C31_8EDB_4CBF_8E59_BED7FE26D6A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "acmcodec.h"

class CG7231Codec : public CACMCodec
{
public:
	int Decode(IN PBYTE pBuf,IN int dLen,OUT int *retLen);
	void Close();
	BOOL Decode(IN PBYTE pBuf,IN int length,OUT PBYTE pOutBuf);
	int Encode(PBYTE buffer=NULL,int size=0);
	//int bufferSize;
	//BYTE dataBuffer[256];
	int Open(int bEncOpen=1); //
	CG7231Codec();
	virtual ~CG7231Codec();

};

#endif // !defined(AFX_G7231CODEC_H__FA593C31_8EDB_4CBF_8E59_BED7FE26D6A4__INCLUDED_)
