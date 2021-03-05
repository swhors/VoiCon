// Wave.h: interface for the CWave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVE_H__949ECD63_96DF_4BBF_8A58_957C42C7F214__INCLUDED_)
#define AFX_WAVE_H__949ECD63_96DF_4BBF_8A58_957C42C7F214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include <Mmreg.h>
#include <io.h>


typedef struct _WAVEDATAFORM
{
	_WAVEDATAFORM();
	~_WAVEDATAFORM();
	_WAVEDATAFORM	&operator=(const _WAVEDATAFORM &d);
	int				operator==(const _WAVEDATAFORM &w);
	void init();
	char _riff[4];
	DWORD dwDataLength;
	char _wave[4];
	char _fmt[4];
	DWORD dwFormatSize;
	WAVEFORMATEX wf;
	WORD  wExtraData;
	char _data[4];
} WAVEDATAFORM,*PWAVEDATAFORM;

class CWave  
{
public:
	CWave(char *szFileName=NULL);
	virtual ~CWave();
	// operator
	CWave			&operator=(const CWave &w);
	int				operator==(const CWave &w);
	int				operator==(const char *fname);
	int				operator<(const char *fname);
	int				operator>(const char *fname);
	// operation
	void			free();
	void			init();
	virtual int		open(IN char *fileName=NULL,IN int bRead=1);
	int				Write(IN unsigned char *inBuf=NULL);
	int				Write(IN unsigned char *inBuf,IN int len);
	int				Read(IN unsigned char *outBuf=NULL);
	virtual void	close();
	int				GetFileHandle();
	void			SetFileHandle(IN int _handle=0);
	char			*GetSRFileName(OUT char *outBuf=NULL);
	void			SetSRFileName(IN char *inBuf);
	char			*GetDESFileName(OUT char *outBuf=NULL);
	void			SetDESFileName(IN char *inBuf);
	void			GetWaveInfo(OUT WAVEDATAFORM &i);
	void			SetWaveInfo(IN	WAVEDATAFORM &i);
	unsigned char	*GetData();
	void			SetData(IN unsigned char *pData,IN int len);
	int				GetDataLen();
	void			SetDataLen(int _len);
private:
	char			m_srFileName[256];
	char			m_desFileName[256];
	int				m_Handle;
	unsigned char	*m_pData;
	int				m_iDatalen;
	WAVEDATAFORM	m_WaveInfo;
};

__inline int is_emptybuf(char *a, int b)
{
	int i;
	for(i = 0; i< b; i++)
	{
		if( a[i] < 0 )
			return 0;
	}
	return 1;
}

__inline int is_valid_wf(WAVEDATAFORM a)
{
	if( a.dwDataLength <= 0 ) return 0;
	if( a.dwFormatSize <= 0 ) return 0;
	if( !is_emptybuf(a._riff,4) ) return 0;
	if( !is_emptybuf(a._wave,4) ) return 0;
	if( !is_emptybuf(a._data,4) ) return 0;
	if( !is_emptybuf(a._fmt,4 ) ) return 0;
	return 1;
}

#endif // !defined(AFX_WAVE_H__949ECD63_96DF_4BBF_8A58_957C42C7F214__INCLUDED_)
