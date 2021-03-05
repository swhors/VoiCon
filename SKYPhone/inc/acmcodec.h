//
#pragma once

#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "WaveDef.h"

class CACMCodec// : public CObject
{
public:	
    CACMCodec();
    virtual	~CACMCodec();

	DWORD		dwOutType;
	DWORD		dwInType;
	PBYTE		dataBuffer;
    int			bufferSize;
	BOOL		bStart;
	
    int			Open( int directon );
    void		Close();
	int			Encode(IN PBYTE buffer,IN int size );
    BOOL		Decode(IN PBYTE buffer,IN int length,OUT int *retlen );	    
	void		build_g7231(IN PWAVEFORMEX pWaveForm);
	void		build_pcm(IN PWAVEFORMEX pWaveForm);
private:
    HACMSTREAM	m_hStream;
};


