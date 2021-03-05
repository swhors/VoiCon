// Wave.cpp: implementation of the CWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wave.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_WAVEDATAFORM::_WAVEDATAFORM()
{
	init();
}

_WAVEDATAFORM::~_WAVEDATAFORM()
{

}

void _WAVEDATAFORM::init()
{
	memset(this->_data,0,4);
	memset(this->_riff, 0, 4);
	memset(this->_wave, 0, 4);
	memset(this->_fmt, 0, 4);
	this->dwDataLength = 0;
	this->dwFormatSize = 0;
	this->wExtraData = 0;
	memset(&this->wf, 0, sizeof(WAVEFORMATEX));
}

_WAVEDATAFORM &_WAVEDATAFORM::operator =(const _WAVEDATAFORM &d)
{
	wsprintf(this->_data, "%s", d._data);
	wsprintf(this->_fmt, "%s", d._fmt);
	wsprintf(this->_riff, "%s", d._riff);
	wsprintf(this->_fmt, "%s", d._fmt);
	this->dwDataLength = d.dwDataLength;
	this->dwFormatSize = d.dwFormatSize;
	this->wExtraData = d.wExtraData;
	memcpy(&this->wf, &d.wf, sizeof(WAVEFORMATEX));
	return *this;
}

int _WAVEDATAFORM::operator ==(const _WAVEDATAFORM &w)
{
	if( !memcmp(this->_data, w._data, 4) )
	{
		if( !memcmp(this->_fmt, w._fmt, 4))
		{
			if( !memcmp(this->_riff, w._riff, 4 ))
			{
				if( !memcmp(this->_wave, w._wave, 4 ))
				{
					if( this->dwDataLength == w.dwDataLength )
					{
						if( this->dwFormatSize == w.dwFormatSize )
						{
							if( this->wExtraData == w.wExtraData )
							{
								if( !memcmp(&this->wf, &w.wf , sizeof(WAVEFORMATEX )) )
								{
									return 1;
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

CWave::CWave(char *szFileName)
{
	init();
	if( szFileName )
		wsprintf(this->m_srFileName, "%s", szFileName);

}

CWave::~CWave()
{
	if( this->m_pData )
		delete this->m_pData;
	this->m_pData = NULL;

}

int CWave::operator ==(const CWave &w)
{
	if( !memcmp(this->m_srFileName, w.m_srFileName, strlen(w.m_srFileName)))
	{
		if( this->m_WaveInfo == w.m_WaveInfo)
			return 1;
	}
	return 0;
}

int CWave::operator ==(const char *fname)
{
	if( !memcmp(this->m_srFileName, fname, strlen(fname )) )
		return 1;
	return 0;
}

int CWave::operator <(const char *fname)
{
	if( memcmp(this->m_srFileName, fname, strlen(fname )) <0 )
		return 1;
	return 0;
}

int CWave::operator >(const char *fname)
{
	if( memcmp(this->m_srFileName, fname, strlen(fname )) >0 )
		return 1;
	return 0;
}

int CWave::GetFileHandle()
{
	return this->m_Handle;
}

void CWave::init()
{
	memset(this->m_srFileName, 0, 256);
	memset(this->m_desFileName, 0, 256);
	this->m_Handle = 0;
	memset(&this->m_WaveInfo,0,sizeof(WAVEDATAFORM));
	this->m_pData = NULL;
}

void CWave::SetFileHandle(IN int _handle)
{
	this->m_Handle = _handle;
}

char *CWave::GetDESFileName(OUT char *outBuf)
{
	if( outBuf)
	{
		wsprintf(outBuf, "%s", this->m_desFileName);
	}
	return this->m_desFileName;

}
void CWave::SetDESFileName(IN char *inBuf)
{
	wsprintf(this->m_desFileName, "%s", inBuf);
}

char *CWave::GetSRFileName(OUT char *outBuf)
{
	if( outBuf)
	{
		wsprintf(outBuf, "%s", this->m_srFileName);
	}
	return this->m_srFileName;
}

void CWave::SetSRFileName(IN char *inBuf)
{
	wsprintf(this->m_srFileName, "%s", inBuf);
}

int CWave::open(IN char *fileName,IN int bRead)
{
	if( fileName )
	{
		wsprintf(this->m_srFileName, "%s", fileName);
	}
	if( strlen(this->m_srFileName ) <= 0 )
	{
		return 0;
	}
	if( bRead )
	{
		if( this->m_Handle = _lopen(this->m_srFileName,OF_READ ) )
			return 1;
	}
	else
	{
		if( this->m_Handle = _lopen(this->m_srFileName,OF_WRITE ) )
			return 1;
	}
	return 0;
}

void CWave::close()
{
	::_lclose(this->m_Handle);
	free();
	init();
}

void CWave::free()
{
	if( this->m_pData )
		delete this->m_pData;
	init();
}

int CWave::Write(IN unsigned char *inBuf,IN int len)
{
	return _lwrite(this->m_Handle, (char*)inBuf, len);
}

int CWave::Write(IN unsigned char *inBuf)
{
	unsigned char *pWriteData;
	
	if( inBuf )
		pWriteData = inBuf;
	else
		pWriteData = this->m_pData;
	
	TRY
	{
		if( !this->m_Handle )
			return 0;
		if( !is_valid_wf(m_WaveInfo) )
			return 0;
		Write((PBYTE)&this->m_WaveInfo._riff, 4);
		Write((PBYTE)&this->m_WaveInfo.dwDataLength, 4);
		Write((PBYTE)&this->m_WaveInfo._wave, 4);
		Write((PBYTE)&this->m_WaveInfo._fmt, 4);
		Write((PBYTE)&this->m_WaveInfo.dwFormatSize, 4);
		Write((PBYTE)&this->m_WaveInfo.wf.wFormatTag, 2);
		Write((PBYTE)&this->m_WaveInfo.wf.nChannels, 2);
		Write((PBYTE)&this->m_WaveInfo.wf.nSamplesPerSec,4);
		Write((PBYTE)&this->m_WaveInfo.wf.nAvgBytesPerSec, 4);
		Write((PBYTE)&this->m_WaveInfo.wf.nBlockAlign, 2);
		Write((PBYTE)&this->m_WaveInfo.wf.wBitsPerSample, 2);
		if( this->m_WaveInfo.dwFormatSize == 18 )
		{
			Write((PBYTE)&this->m_WaveInfo.wf.cbSize, 2);
		}
		Write((PBYTE)&this->m_WaveInfo._data, 4);
		Write(this->m_pData, this->m_iDatalen);
	}
	CATCH(CFileException, e)
	{
		return 0;
	}
	CATCH(CMemoryException , e)
	{
		return 0;
	}
	CATCH(CException, e)
	{
		return 0;
	}
	END_CATCH
	return 1;
}

int CWave::Read(IN unsigned char *outBuf)
{
	TRY
	{
		if( !this->m_Handle )
			return 0;
		_lread(this->m_Handle, this->m_WaveInfo._riff, 4);
		//전체 파일 크기에서 8Byte를 제외한 크기
		//len = (48 - 8) + dwSaveBufLength;
		_lread(this->m_Handle, &this->m_WaveInfo.dwDataLength, 4);
		this->m_iDatalen = this->m_WaveInfo.dwDataLength -40;
		_lread(this->m_Handle, &this->m_WaveInfo._wave, 4);
		_lread(this->m_Handle, &this->m_WaveInfo._fmt, 4);
		_lread(this->m_Handle, &this->m_WaveInfo.dwFormatSize, 4);
		_lread(this->m_Handle, &this->m_WaveInfo.wf.wFormatTag, 2);
		_lread(this->m_Handle, &this->m_WaveInfo.wf.nChannels, 2);
		_lread(this->m_Handle, &this->m_WaveInfo.wf.nSamplesPerSec,4);
		_lread(this->m_Handle, &this->m_WaveInfo.wf.nAvgBytesPerSec, 4);
		_lread(this->m_Handle, &this->m_WaveInfo.wf.nBlockAlign, 2);
		_lread(this->m_Handle, &this->m_WaveInfo.wf.wBitsPerSample, 2);
		if( this->m_WaveInfo.dwFormatSize == 18 )
		{
			// 일반적인 포멧의 사이즈는 16이며, 여분의 데이터가 있는 경우
			// 이 크기는 증가한다. 증가된 이 값은 cdSize이다.
			_lread(this->m_Handle, &this->m_WaveInfo.wf.cbSize, 2);
		}
		_lread(this->m_Handle, &this->m_WaveInfo._data, 4);
		
		if( this->m_iDatalen > 0 )
		{
			this->m_pData = new unsigned char[this->m_iDatalen + 1];
			memset(this->m_pData, 0, this->m_iDatalen+1 );
			_lread(this->m_Handle, this->m_pData, this->m_iDatalen);
		}
	}
	CATCH(CFileException, e)
	{
		return 0;
	}
	CATCH(CMemoryException, e)
	{
		return 0;
	}
	CATCH(CException, e)
	{
		return 0;
	}
	END_CATCH
	return 1;
}

unsigned char *CWave::GetData()
{
	return this->m_pData;
}

void CWave::SetData(IN unsigned char *pData,IN int len)
{
	if( !pData || len <=0 )
		return;
	this->m_pData = new unsigned char[len+1];
	memcpy(this->m_pData, pData, len);
}

void CWave::SetDataLen(int _len)
{
	this->m_iDatalen = _len;
}

int CWave::GetDataLen()
{
	return this->m_iDatalen;
}
