// ACMWAVE.h: interface for the CACMWAVE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACMWAVE_H__6DA5EB1A_AF1D_4900_895A_87802A2B2EF3__INCLUDED_)
#define AFX_ACMWAVE_H__6DA5EB1A_AF1D_4900_895A_87802A2B2EF3__INCLUDED_

//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000

#include "Wave.h"
#include "msacm.h"
#include <io.h>

#define WAVE_FORMAT_ON_G723	66 // Microsoft g.723

#define ACM_OUT_NONE	0x0000
#define ACM_OUT_WAVE	0x0010
#define ACM_OUT_G7231	0x0020
#define ACM_OUT_G711U	0x0030
#define ACM_OUT_G711A	0x0040
#define ACM_OUT_GSM		0x0050

#define ACM_IN_NONE		0x0000
#define ACM_IN_WAVE		0x0100
#define ACM_IN_G7231	0x0200
#define ACM_IN_G711U	0x0300
#define ACM_IN_G711A	0x0400
#define ACM_IN_GSM		0x0500

#ifndef G7231FORMAT
typedef struct _G7231FORMAT
{
	WAVEFORMATEX wf;
	BYTE extra[10];
} G7231FORMAT,*PG7231FORMAT;
#endif

#ifndef DRIVERINFO
typedef struct _DRIVERINFO
{
	HACMDRIVERID hadid;
	WORD wFormatTag;
	WAVEFORMATEX* pWf;
}DRIVERINFO,*PDRIVERINFO;
#endif

class CACMWAVE : public CWave
{
public:
	CACMWAVE(char *fName=NULL);
	virtual ~CACMWAVE();
	virtual int				open(IN char *fileName=NULL,IN int bRead=1);
	int						acm_open();
	void					acm_close();
	int						acm_convert();
	int						write_raw();
	int						write_wave();
	int						convert();
	virtual void			close();
	void					EnableOutG711U();
	void					EnableOutG711A();
	void					EnableOutG7231();
	void					EnableOutGSM();
	void					EnableOutWave();
	void					EnableInG711U();
	void					EnableInG711A();
	void					EnableInG7231();
	void					EnableInGSM();
	void					EnableInWave();
public:
	static BOOL CALLBACK	FindFormatEnum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport);
	static BOOL CALLBACK	DriverEnumProc(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport);
	HACMDRIVERID			AcmFindCodecDriver(WORD wFormatTag);
private:
	int						bAcm;	// if true, wave -> acm convert.
									// else if false, acm -> wave convert.
	PBYTE					pOutBuf;
	int						pOutBufLen;
	int						CodedLen;
	
	HACMDRIVERID			DeviceId;
	HACMDRIVERID			pcmID;
	HACMSTREAM				m_hStream;
	HACMDRIVER				m_hDriver;
	DRIVERINFO				driverInfo;	
	WAVEFORMATEX			pcmfmt;
	DWORD					dwOutType;
	DWORD					dwInType;
	//wave play info file
	//WAVE_PLAY_INFO	playInfo;
};

#endif // !defined(AFX_ACMWAVE_H__6DA5EB1A_AF1D_4900_895A_87802A2B2EF3__INCLUDED_)
