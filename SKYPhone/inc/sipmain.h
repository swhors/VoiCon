// sipmain.h: interface for the CSIPMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIPMAIN_H__7BABC2D5_8162_4F37_925B_78B46A4E9480__INCLUDED_)
#define AFX_SIPMAIN_H__7BABC2D5_8162_4F37_925B_78B46A4E9480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _REAL_RTP_USE

#include "SkySipUaManager.h"

#ifdef _REAL_RTP_USE

#include "pcmwave.h"
#include "usermsg.h"
#include <mmsystem.h>

#define		OUT_WAVE		1
#define		IN_WAVE			2
#define		BUSY_WAVE		3
#endif // _REAL_RTP_USE

class CSIPMain : public sky_sip_manager  
{
public:
	CSIPMain();
	virtual ~CSIPMain();
public:// function.
	void voice_stop();
	int voice_start(HWND hWndDlg);
	void PSetNum(char inputChar);
	void ClearNum();
	void DelNum();
public:// Virtual Event Message Function
	virtual int  OnRecvNewCall(void * pArg);
	virtual int  OnRecvEndCall(void * pArg);
	virtual int  OnRecvReqCallOk(void * pArg);
	virtual int  OnRecvNewCallAck(void *pArg);
	virtual int  OnRecvCancelCall(void *pArg);
	virtual int  OnRecvRegisterOk(void *pArg);
	virtual int  OnRecvFailMsg(void *parg);
	virtual int  OnSendReqCall(void *pArg);
	virtual int  OnSendNewCallOk(void *pArg);
	virtual int  OnSendReqCallAck(void *pArg);
	virtual int  OnSendRegister(void *pArg);
	virtual int  OnSendFailMsg(void *pArg);
	virtual int  OnSendEndCall(void *pArg);
	virtual int  OnFailNewCall(void *pArg);
	virtual int  OnFailReqCall(void *pArg);
	virtual int  OnRecvTrying(void *pArg);
	virtual int  OnRecvRinging(void *pArg);
	virtual int  OnSendTrying(void *pArg);
	virtual int  OnSendRinging(void *pArg);
	virtual void OnRecvSipInfoMsg(IN void *pArg);
	// By OK
	virtual void OnRecvMsgByeOK();
	// timer event
	virtual void OnTimer(IN UINT nIDEvent);
	// rtp event
	int  OnRtpInOpen(IN int port);
	int  OnRtpOutOpen(IN int port);
	int  OnRtpOutSetInfo(IN char *ip,IN int port);
	int  OnRtpInSetInfo(IN char *ip,IN int port);
	int  OnRtpOpenChannel(IN int icodec);
	int  OnRtpCloseChannel(IN int icodec);
	void OnRtpInClose();
	void OnRtpOutClose();
	void SendDtmf(char dtmf);
    void OnRegisterFail(void * parg);

public:// voice function
#ifdef _REAL_RTP_USE
	MMRESULT SetVolume(DWORD LineType, int Volume);
	int		 SetValue(MIXERCONTROL& mxc, int Volume, int Mode);
    int      LoadWaveFile(LPSTR filename, int nMode);
	int      wAllocMemory(int nMode);
	void     wFreeMemory();
#endif // _REAL_RTP_USE
#ifdef _REAL_RTP_USE
public: // rtp information
	time_t     call_start;
    struct tm  tm2;
	char	precalledNum[50];
	char    sdp_msg[512];
	HMIXER		hmx;
	MIXERLINE	mxl;
	DWORD		m_dwChannels;
	int			OldMasterVolume, OldWaveOutVolume;
	PCMWAVEFORMAT			WaveRecord;
	HANDLE					waveOutDataBlock;
	HANDLE					waveInDataBlock;
	HANDLE					waveBusyDataBlock;
	long					lOutDatasize;
	long					lInDatasize;
	long					lBusyDatasize;
	LPBYTE					pOutWave;
	LPBYTE					pInWave;
	LPBYTE					pBusyWave;
	CPCMWave *USBwave;
	CPCMWave *USBOutwave;
	CPCMWave *SPwave;	
	CPCMWave *BUSYwave;
	int						erroCode;
	MMCKINFO				MMCkInfoParent;
	MMCKINFO				MMCkInfoChild;
	int                     busystart;
#endif //_REAL_RTP_USE
};

#endif // !defined(AFX_SIPMAIN_H__7BABC2D5_8162_4F37_925B_78B46A4E9480__INCLUDED_)
