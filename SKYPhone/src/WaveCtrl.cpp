// WaveCtrl.cpp: implementation of the CWaveCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ubidialer.h"
#include "WaveCtrl.h"

#include "stdafx.h"
#include "resource.h"
#include "UBIDialerDlg.h"
#include "UBIDialer.h"

#include "mediathread.h"
#include "pcmwave.h"
#include "usermsg.h"
#include "netutils.h"
HWND gMainWnd = NULL;

#include "time.h"
#include "pcmwave.h"
#include "phonevoice.h"
#include "AudioInterface.h"

extern BOOL bplay1;
extern BOOL bRTPSendOff;

extern CUbiDialerApp theApp;

extern int waitRtpPort;

extern CAudioInterface *gUserWaveIn, *gUserWaveOut;

void send_media(void *rData, int len, int type, int opt);

extern unsigned int gUsedCodec;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaveCtrl::CWaveCtrl()
{

}

CWaveCtrl::~CWaveCtrl()
{

}

BOOL CWaveCtrl::VoiceStart(int iRtpPort,HWND hWndDlg)
{
	CString		strTemp;
	precalledNum[0] = 0;
	USBwave = NULL;
	USBOutwave = NULL;
	SPwave = NULL;
	BUSYwave = NULL;

	//PrintCallMsg("로그인중...");

	//callstt = CALL_IDLE;
	USBwave = new CPCMWave;
	USBOutwave = new CPCMWave;
	SPwave = new CPCMWave;
	BUSYwave = new CPCMWave;
	waitRtpPort = iRtpPort;
	if( !hWndDlg )
	{
		gMainWnd = AfxGetApp()->m_pMainWnd->m_hWnd;;
		StartMediaThread(gMainWnd);
	}
	else
	{
		StartMediaThread(hWndDlg);
		gMainWnd = hWndDlg;
	}
#if 1
	if(USBwave)
	{
		USBwave->open(1, 16, 8000, 480);
	}

	if(USBOutwave)
	{
		strTemp.Format("%s","ringout.wav");
		LoadWaveFile(strTemp.GetBuffer(strTemp.GetLength()),OUT_WAVE);
		USBOutwave->selectOutDev("UBI");
		USBOutwave->open(1, 8, 11025, lOutDatasize);
	}

	if(SPwave)
	{
		strTemp.Format("%s","ringin.wav");
		LoadWaveFile(strTemp.GetBuffer(strTemp.GetLength()), IN_WAVE);
		SPwave->open(1, 8, 11025, lInDatasize);
	}

	if(BUSYwave)
	{
		strTemp.Format("%s","busy.wav");
		LoadWaveFile(strTemp.GetBuffer(strTemp.GetLength()), BUSY_WAVE);
		BUSYwave->open(1, 8, 11025, lBusyDatasize);
//		busycount = 0;
	}
#endif
	return TRUE;
}

void CWaveCtrl::VoiceStop()
{
	::KillTimer(gMainWnd, 10);
	::KillTimer(gMainWnd, 20);	
	::KillTimer(gMainWnd, 50);	

	if(gMediaThrID) ::SendMessage(gMediaWnd, UM_MEDIAQUIT, 0, 0);
	if(gMediaThrID) ::PostThreadMessage(gMediaThrID, WM_QUIT, 0, 0);
	if(ghMediaThr) ::WaitForSingleObject((HANDLE)ghMediaThr, 1000);
	wFreeMemory();	

	if(USBwave)
	{
		USBwave->close();
		delete USBwave;
		USBwave = NULL;
	}

	if(USBOutwave)
	{
		USBOutwave->close();
		delete USBOutwave;
		USBOutwave = NULL;
	}

	if(SPwave)
	{
		SPwave->close();
		delete SPwave;
		SPwave = NULL;
	}

	if(BUSYwave)
	{
		BUSYwave->close();
		delete BUSYwave;
		BUSYwave = NULL;
	}
}

void CWaveCtrl::wFreeMemory()
{
	if(waveOutDataBlock)
	{
		::GlobalUnlock(waveOutDataBlock);
		::GlobalFree(waveOutDataBlock);
		waveOutDataBlock=NULL;
		pOutWave=NULL;
	}

	if(waveInDataBlock)
	{
		::GlobalUnlock(waveInDataBlock);
		::GlobalFree(waveInDataBlock);
		waveInDataBlock=NULL;
		pInWave=NULL;
	}		

	if(waveBusyDataBlock)
	{
		::GlobalUnlock(waveBusyDataBlock);
		::GlobalFree(waveBusyDataBlock);
		waveBusyDataBlock=NULL;
		pBusyWave=NULL;
	}
}

int CWaveCtrl::wAllocMemory(int nMode)
{
	switch(nMode)
	{
	case OUT_WAVE:
		waveOutDataBlock = ::GlobalAlloc(GMEM_MOVEABLE,lOutDatasize);
		if(waveOutDataBlock == NULL)
			return FALSE;
		pOutWave = (LPBYTE)::GlobalLock(waveOutDataBlock);	
		break;
	case IN_WAVE:			
		waveInDataBlock = ::GlobalAlloc(GMEM_MOVEABLE,lInDatasize);
		if(waveInDataBlock == NULL)
			return FALSE;
		pInWave = (LPBYTE)::GlobalLock(waveInDataBlock);		
		break;
	case BUSY_WAVE:		
		waveBusyDataBlock = ::GlobalAlloc(GMEM_MOVEABLE,lBusyDatasize);
		if(waveBusyDataBlock == NULL)
			return FALSE;

		pBusyWave = (LPBYTE)::GlobalLock(waveBusyDataBlock);

	}
	return TRUE;

}

int CWaveCtrl::LoadWaveFile(LPSTR filename, int nMode)
{
	char strFileName[80];
	long	ll_temp;

	memset(strFileName,0,sizeof(strFileName));

	lstrcpy((LPSTR)strFileName,(LPSTR)filename);

	HMMIO hmmio = mmioOpen((LPSTR)strFileName,NULL,MMIO_READ);

	if(!hmmio)
	{
		AfxMessageBox("화일이 없습니다.!");
		return FALSE;
	}

	MMCkInfoParent.fccType = mmioFOURCC('W','A','V','E');
	erroCode = mmioDescend(hmmio,&MMCkInfoParent,NULL,MMIO_FINDRIFF);
	
	if(erroCode)
	{
		AfxMessageBox("웨이브화일이 아닙니다.");
		mmioClose(hmmio,0);
		return FALSE;
	}
	
	MMCkInfoChild.ckid = mmioFOURCC('f','m','t',' '); 
	erroCode = mmioDescend(hmmio,&MMCkInfoChild,&MMCkInfoParent,MMIO_FINDCHUNK);
	
	if(erroCode)
	{
		AfxMessageBox("fmt Descending Error");
		mmioClose(hmmio,0);
		return FALSE;
	}

	DWORD bytesRead = mmioRead(hmmio,(LPSTR)&WaveRecord,MMCkInfoChild.cksize);
	
	if(bytesRead<=0)
	{
		AfxMessageBox("wave format read Error");
		mmioClose(hmmio,0);
		return FALSE;
	}
	
	erroCode = mmioAscend(hmmio,&MMCkInfoChild,0);
	
	if(erroCode)
	{
		AfxMessageBox("Error Ascending in file");
		mmioClose(hmmio,0);
		return FALSE;
	}

	MMCkInfoChild.ckid = mmioFOURCC('d','a','t','a');

	erroCode = mmioDescend(hmmio,&MMCkInfoChild,&MMCkInfoParent,MMIO_FINDCHUNK);

	if(erroCode)
	{
		AfxMessageBox("data 청크를 찾을수 가 없습니다.");
		mmioClose(hmmio,0);
		return FALSE;
	}

	switch(nMode)
	{
	case OUT_WAVE:
		lOutDatasize = MMCkInfoChild.cksize;
		if(!wAllocMemory(nMode))
		{
			AfxMessageBox("적재할 메모리가 부족합니다");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		if((ll_temp = mmioRead(hmmio,(LPSTR)pOutWave,lOutDatasize)) != lOutDatasize)
		{
			AfxMessageBox("데이타를 읽을수가 없습니다.");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		break;
	case IN_WAVE:		
		lInDatasize = MMCkInfoChild.cksize;
		if(!wAllocMemory(nMode))	{
			AfxMessageBox("적재할 메모리가 부족합니다");
			mmioClose(hmmio,0);
			return FALSE;
		}
		if((ll_temp = mmioRead(hmmio,(LPSTR)pInWave,lInDatasize)) != lInDatasize)
		{
			AfxMessageBox("데이타를 읽을수가 없습니다.");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		break;	
	case BUSY_WAVE:
		lBusyDatasize = MMCkInfoChild.cksize;
		if(!wAllocMemory(nMode))
		{
			AfxMessageBox("적재할 메모리가 부족합니다");
			mmioClose(hmmio,0);
			return FALSE;
		}
		if((ll_temp = mmioRead(hmmio,(LPSTR)pBusyWave,lBusyDatasize)) != lBusyDatasize)
		{
			AfxMessageBox("데이타를 읽을수가 없습니다.");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		break;	
	}

	mmioClose(hmmio,0);	 

	return TRUE;
}

int CWaveCtrl::SetValue(MIXERCONTROL &mxc, int Volume, int Mode)
{
	MMRESULT		mmresult;
	MIXERCONTROLDETAILS	mxcd={0};
	MIXERCONTROLDETAILS_UNSIGNED	mcd_u = {0};
	int			ret = 0;

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cChannels = m_dwChannels;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mcd_u;

	mmresult = mixerGetControlDetails((HMIXEROBJ)hmx,&mxcd,0L);
	if(mmresult == MMSYSERR_NOERROR)
	{
		char szValue[10];
		sprintf(szValue, "%d", mcd_u.dwValue);		
		
		if (Mode == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS)
			ret = mcd_u.dwValue;
		else if (Mode == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT)
			ret = mcd_u.dwValue;

		mcd_u.dwValue = Volume;
		mmresult = mixerSetControlDetails((HMIXEROBJ)hmx,&mxcd,0L);
	}	
	return ret;
}
