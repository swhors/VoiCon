// WaveCtrl.h: interface for the CWaveCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVECTRL_H__C39F18F9_47B4_4D2F_969D_F313A75DB965__INCLUDED_)
#define AFX_WAVECTRL_H__C39F18F9_47B4_4D2F_969D_F313A75DB965__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pcmwave.h"
#include "usermsg.h"
#include <mmsystem.h>

#define		OUT_WAVE		1
#define		IN_WAVE			2
#define		BUSY_WAVE		3

class CWaveCtrl  
{
public:
	int SetValue(MIXERCONTROL& mxc, int Volume, int Mode);
	int LoadWaveFile(LPSTR filename, int nMode);
	int wAllocMemory(int nMode);
	void wFreeMemory();
	void VoiceStop();
	//BOOL VoiceStart(IN int iRtpPort=10000,IN HWND phWnd=NULL);
	BOOL VoiceStart(IN int iRtpPort,IN HWND phWnd=NULL);
	CWaveCtrl();
	virtual ~CWaveCtrl();

	time_t				call_start;
    struct				tm  tm2;
	char				precalledNum[50];
	HMIXER				hmx;
	MIXERLINE			mxl;
	DWORD				m_dwChannels;
	int					OldMasterVolume, OldWaveOutVolume;
	PCMWAVEFORMAT		WaveRecord;
	HANDLE				waveOutDataBlock;
	HANDLE				waveInDataBlock;
	HANDLE				waveBusyDataBlock;
	long				lOutDatasize;
	long				lInDatasize;
	long				lBusyDatasize;
	LPBYTE				pOutWave;
	LPBYTE				pInWave;
	LPBYTE				pBusyWave;
	CPCMWave			*USBwave;
	CPCMWave			*USBOutwave;
	CPCMWave			*SPwave;
	CPCMWave			*BUSYwave;
	int					erroCode;
	MMCKINFO			MMCkInfoParent;
	MMCKINFO			MMCkInfoChild;
	int					busystart;
};

#endif // !defined(AFX_WAVECTRL_H__C39F18F9_47B4_4D2F_969D_F313A75DB965__INCLUDED_)
