// sipmain.cpp: implementation of the CSIPMain class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Definition of Include
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "UBIDialerDlg.h"
#include "UBIDialer.h"
#include "sipmain.h"
#include "SkySipUaManager.h"

#include "dtmf_def.h"

#include "netutils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _REAL_RTP_USE
#include "mediathread.h"
#include "pcmwave.h"
#include "usermsg.h"
#include "netutils.h"
HWND gMainWnd = NULL;
#endif// _REAL_RTP_USE
#include "time.h"
#include "sip_header.h"
#include "sipmain.h"
#include "pcmwave.h"
#include "phonevoice.h"
#include "AudioInterface.h"

extern BOOL bplay1;
extern BOOL bRTPSendOff;

extern CUbiDialerApp theApp;

extern CAudioInterface *gUserWaveIn, *gUserWaveOut;

void send_media(void *rData, int len, int type, int opt);

extern unsigned int gUsedCodec;
int get_rtp_handle(int dir);

int busycount;
//////////////////////////////////////////////////////////////////
// Definition of Function
//////////////////////////////////////////////////////////////////

void SendStateMsg2(unsigned int wParam, char * sBuf, unsigned int lParam)
{
	char * sbuff=NULL;
#ifdef WIN32
		CUbiDialerDlg * pDlg;
#endif
	if( sBuf )
	{
	    sbuff = (char*) malloc(strlen(sBuf)+1);
		if( sbuff )
	        sprintf(sbuff,"%s", sBuf);
	}
#ifdef WIN32
	if( theApp )
	{
		if( theApp.m_pMainWnd )
		{
			pDlg = (CUbiDialerDlg *)theApp.m_pMainWnd;
			if( sbuff )
			    pDlg->SendMessage(WM_USER_MSG,(WPARAM)wParam,(LPARAM)sbuff);
			else
                pDlg->SendMessage(WM_USER_MSG,(WPARAM)wParam,(LPARAM)lParam);
		}
	}
#elif defined LINUX
	if( sbuff )
	    free(sbuff);
#endif
}

void SendStateMsg(char * sBuf)
{
	//time_t tt;
	//struct tm * ltm;
	char * sbuff;
	sbuff = (char*) malloc(512);
	if(sbuff )
	{
		//time(&tt);
		//ltm = localtime(&tt);
		//sprintf(sbuff, "%02d/%02d/%02d : %s",
		//	      ltm->tm_hour,ltm->tm_min,ltm->tm_sec,sBuf);
		sprintf(sbuff,"%s", sBuf);
#ifdef WIN32
		CUbiDialerDlg * pDlg;
		if( theApp )
		{
			if( theApp.m_pMainWnd )
			{
				pDlg = (CUbiDialerDlg *)theApp.m_pMainWnd;
				pDlg->SendMessage(WM_USER_MSG,(WPARAM)sbuff);
			}
		}
#elif defined LINUX
		free(sbuff);
#endif
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSIPMain::CSIPMain()
{
	busystart = 0;
	call_start = 0;
}

CSIPMain::~CSIPMain()
{
}

void CSIPMain::PSetNum(char inputChar)
{
	if( CallInfo.eCallState == e_call_ready
		|| CallInfo.eCallState == e_call_end)
	{
	    char tbuf[32];
	    sprintf(tbuf, "%c", inputChar);
	    strcat(CallInfo.target_number,tbuf);
	}
	else
	{
		if( CallInfo.eCallState == e_call_success
			&& CallInfo.m_call_transfer == 1)
		{
			char tbuf[32];
	        sprintf(tbuf, "%c", inputChar);
	        strcat(ReferInfo.refer_to_contact_num,tbuf);
		}
	}
}

void CSIPMain::ClearNum()
{
	memset(CallInfo.target_number, 0, 64);

}

void CSIPMain::DelNum()
{
	if( CallInfo.eCallState == e_call_ready
		|| CallInfo.eCallState == e_call_end)
	{
	    int len = sizeof( CallInfo.target_number);
	    CallInfo.target_number[len-1]=0;
	}
	else
	{
		if( CallInfo.eCallState == e_call_success
			&& CallInfo.m_call_transfer == 1)
		{
	        int len = sizeof( ReferInfo.refer_to_contact_num);
	        ReferInfo.refer_to_contact_num[len-1]=0;
		}
	}
}

int CSIPMain::OnRecvNewCall(void * pArg)
{
	p_sip_hd_t phd = (p_sip_hd_t)pArg;
	SendStateMsg2(WM_SIPMSG_RV_INV,phd->sipfield.from.uri.user_number.str, 0);
	return 1;
}

int CSIPMain::OnRecvEndCall(void * pArg)
{
	SendStateMsg2(WM_SIPMSG_RV_BYE, NULL, 0);
	return 1;
}

int CSIPMain::OnRecvReqCallOk(void * pArg)
{
	USBOutwave->bInTalk = TRUE;
    USBwave->bInTalk = TRUE;
	KillTimer(gMainWnd, 10);
	KillTimer(gMainWnd, 20);
	SendStateMsg2(WM_SIPMSG_SD_INV_SUCC,NULL, 0);
	return 1;
}

int CSIPMain::OnRecvNewCallAck(void *pArg)
{
	SendStateMsg2(WM_SIPMSG_RV_INV_SUCC,NULL,0);
	return 1;
}

int CSIPMain::OnRecvCancelCall(void *pArg)
{
	SendStateMsg2(WM_SIPMSG_RV_INV_FAIL,NULL,0);
	return 1;
}

int CSIPMain::OnRecvRegisterOk(void *pArg)
{
	SendStateMsg2(WM_SIPMSG_SD_REG_SUCC,NULL,0);
	return 1;
}


void CSIPMain::OnRegisterFail(PVOID pArg)
{
	SendStateMsg2(WM_SIPMSG_SD_REG_FAIL,NULL,0);
}

int CSIPMain::OnRecvFailMsg(void *parg)
{
	int error_code = (int)parg;	
	switch( error_code )
	{
	case 484:
	case 404: // not found
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		// play dial tone
		break;
	case 408: // time out
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		break;
	case 400:
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		break;
	case 401: // req WWW Auth
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		break;
	case 407: // www auth fail
		break;
	case 405: // unsupported method
		SendStateMsg2(WM_SIPMSG_UNSUPP_METHOD,NULL, error_code);
		break;
	case 486: // busy
		if( !busystart )
		{
			busystart = 1;
		    SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		    KillTimer(gMainWnd, 10);
		    KillTimer(gMainWnd, 20);
		    SetTimer(gMainWnd, 50, 20, NULL);
		}
		break;
	default:
	    SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		break;
	}
	return 1;
}

int CSIPMain::OnSendReqCall(void *pArg)
{
    SendStateMsg2(WM_SIPMSG_SD_INV,NULL, 0);
	return 1;
}

int CSIPMain::OnSendNewCallOk(void *pArg)
{
	USBOutwave->bInTalk = TRUE;
    USBwave->bInTalk = TRUE;
	KillTimer(gMainWnd, 10);
	KillTimer(gMainWnd, 20);
    SendStateMsg2(WM_SIPMSG_SD_NEW_CALL_OK,NULL,0);
	return 1;
}

int CSIPMain::OnSendReqCallAck(void *pArg)
{

	return 1;
}

int CSIPMain::OnSendRegister(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendFailMsg(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendEndCall(void *pArg)
{
	return 1;
}

// 에러 처리
int CSIPMain::OnFailNewCall(void *pArg)
{
    SendStateMsg2(WM_SIPMSG_RV_INV_FAIL,NULL,0);
	return 1;
}

// 에러 처리
int CSIPMain::OnFailReqCall(void *pArg)
{
    SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL,0);
	return 1;
}

int CSIPMain::OnRecvTrying(void *pArg)
{
	return 1;
}

int CSIPMain::OnRecvRinging(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendTrying(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendRinging(void *pArg)
{
	return 1;
}

void CSIPMain::OnRtpInClose()
{
#ifdef _REAL_RTP_USE
	::SendMessage(gMediaWnd, UM_CLOSEINRTP, 0, 0);
#endif //_REAL_RTP_USE
}

void CSIPMain::OnRtpOutClose()
{
#ifdef _REAL_RTP_USE
	::SendMessage(gMediaWnd, UM_CLOSEOUTRTP, 0, 0);
#endif // _REAL_RTP_USE
}

int CSIPMain::OnRtpInOpen(IN int port)
{
#ifdef _REAL_RTP_USE
	::SendMessage(gMediaWnd, UM_REQINRTP, 0, port);
#endif // _REAL_RTP_USE
	return 1;
}
int CSIPMain::OnRtpOutOpen(IN int port)
{
#ifdef _REAL_RTP_USE
	::SendMessage(gMediaWnd, UM_REQOUTRTP, 0, port);
#endif // _REAL_RTP_USE
	return 1;
}

void CSIPMain::OnTimer(IN UINT nIDEvent)
{
#ifdef _REAL_RTP_USE
	switch(nIDEvent)
	{
	case 9:
		KillTimer(gMainWnd, 9);
		if( stun_enable )
			if( sipUAC )
		        sipUAC->send_udp("    ", 4, proxyip, 5060);
		//if( sipUAC )
		//    sipUAC->send_udp("    ", 4, "210.121.190.30", 5060);
		//else
        //    sipUAC->send_udp("    ", 4, stunInfo.stunip, stunInfo.stunport);
		SetTimer(gMainWnd, 9, 30000,NULL);
		break;
	case 10:
		{
			KillTimer(gMainWnd, 10);
			if( SPwave )
			{
				SPwave = (CPCMWave *)SPwave;
				SPwave->waveOut(pInWave);
			}
			SetTimer(gMainWnd, 10, 6000, NULL);
		}
		break;
	case 20:
		{
			KillTimer(gMainWnd, 20);
			if( USBOutwave )
			{
				USBOutwave = (CPCMWave *)USBOutwave;
				USBOutwave->waveOut(pOutWave);
			}
			SetTimer(gMainWnd, 20, 6000, NULL);
		}
		break;
	case 30:
		break;
	case 40:
		//::KillTimer(gMainWnd, 40);
		//::KillTimer(gMainWnd, 60);
		//::KillTimer(gMainWnd, 70);
		::KillTimer(gMainWnd, 50);
		break;
	case 50:
		{
			KillTimer(gMainWnd, 50);
			if( busycount++ < 12 )
			{
		        if( BUSYwave)
		            BUSYwave->waveOut(pBusyWave);
			    SetTimer(gMainWnd, 50, 1000, NULL);
			}
			else
			{
				SendStateMsg2(WM_SIPMSG_RV_BYE, NULL, 0);
				busycount = 0;
				busystart = 0;
			}
		}
		break;
	case 60:
		//PrintCallMsg( "연결중..." );
		break;
	case 70:
		//PrintCallMsg( "연결됨" );
		break;
	}
#endif // _REAL_RTP_USE
}

int CSIPMain::OnRtpOutSetInfo(IN char *ip,IN int port)
{
#ifdef _REAL_RTP_USE
	unsigned int IIP;

	IIP = ip_to_uint32(ip);
	::SendMessage(gMediaWnd, UM_SETOUTRTCP_RMTADDR, IIP, port+1);
	::SendMessage(gMediaWnd, UM_SETOUTRTP_RMTADDR, IIP, port);
#endif// _REAL_RTP_USE
	return 1;
}

void CSIPMain::OnRecvSipInfoMsg(IN void *pArg)
{
	p_sip_hd_t phd = (p_sip_hd_t)pArg;
	if( phd )
	{
		if( phd->sipfield.content_length> 0)
		{
			if( !memcmp(phd->sipfield.sdp.str, "startCHAT", 9 ))
                SendStateMsg2(WM_SIPMSG_SIP_INFO_RV,NULL,1);
			else
			{
				sprintf(sdp_msg, "%s", phd->sipfield.sdp.str);
	            SendStateMsg2(WM_SIPMSG_SIP_INFO_RV,NULL,0);
			}
		}
	}
}

int CSIPMain::OnRtpInSetInfo(IN char *ip,IN int port)
{
#ifdef _REAL_RTP_USE

	unsigned int IIP;
	IIP = ip_to_uint32(ip);
	::SendMessage(gMediaWnd,UM_SETINRTCP_RMTADDR,IIP,port+1);
	::SendMessage(gMediaWnd,UM_SETINRTP_RMTADDR,IIP,port);
	if( stun_enable )
	{
		Sleep(100);
	}
	return get_rtp_handle(0);
#else
	return 0;
#endif// _REAL_RTP_USE

}


#ifdef _REAL_RTP_USE
int CSIPMain::SetValue(MIXERCONTROL& mxc, int Volume, int Mode)
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

int CSIPMain::LoadWaveFile(LPSTR filename, int nMode)
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

void CSIPMain::wFreeMemory()
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

int CSIPMain::wAllocMemory(int nMode)
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
#endif// _REAL_RTP_USE

int CSIPMain::voice_start(HWND hWndDlg)
{
#ifdef _REAL_RTP_USE
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

	StartMediaThread(hWndDlg);
	gMainWnd = hWndDlg;
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
		busycount = 0;
	}
#endif
#endif // _REAL_RTP_USE
	return 1;
}

void CSIPMain::voice_stop()
{
#ifdef _REAL_RTP_USE
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
#endif // _REAL_RTP_USE
}

int CSIPMain::OnRtpOpenChannel(IN int icodec)
{
	if(gMediaThrID)
	{
		::SendMessage(gMediaWnd, UM_OPENINDEV, icodec, 0);
		Sleep(80);
        ::SendMessage(gMediaWnd, UM_OPENOUTDEV, icodec, 0);
	}
	return 1;
}
int CSIPMain::OnRtpCloseChannel(IN int icodec)
{
	if(gMediaThrID)
	{
		::SendMessage(gMediaWnd, UM_CLOSEINDEV, icodec, 0);
		::SendMessage(gMediaWnd, UM_CLOSEOUTDEV, icodec, 0);
	}
	return 1;
}

void CSIPMain::OnRecvMsgByeOK()
{
    SendStateMsg2(WM_SIPMSG_SD_BYE_SUCC,NULL,0);
}

//
//
void CSIPMain::SendDtmf(char dtmf)
{
	if( CallInfo.eCallState == e_call_success )
	{
		if( CallInfo.m_call_transfer )
			return;
	    switch( dtmf_type )
	    {
	    case dtmf_unknown:
		    break;
	    case dtmf_sipinfo:
	        send_dtmf(dtmf);
		    break;
	    case dtmf_inband:
		    {
				if( gUserWaveOut && bplay1)
				{
				    gUserWaveOut->SendDTMFinBand(dtmf);
				}
		    }
		    break;
	    case dtmf_rfc2833:
			{
				int i;
				DTMF2833PAYLOAD tdtmf;
				bRTPSendOff = TRUE;
				for( i =0 ; i < 10 ; i ++)
				{
					int opt=0;
					make_dtmf_2833_msg(dtmf, i, &tdtmf, 4);
					if( i >= 2 )
						opt = 1;
					else
						opt = 0;
                    send_media(&tdtmf, 4, RTP_DTMF_2833_TYPE, opt);
					Sleep(16);
				}
                bRTPSendOff = FALSE;
			}
		    break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// end of file
//////////////////////////////////////////////////////////////////////////
