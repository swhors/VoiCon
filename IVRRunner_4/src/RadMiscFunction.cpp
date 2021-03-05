#include "stdafx.h"
#include "string.h"
#include "resource.h"
#include "process.h"
#include <stddef.h>
#include <stdlib.h>
#include <conio.h>

#include <q931asn1.h>
#include "rtp.h"
#include "rtcp.h"
#include "operate.h"
#include "ivrkeydata.h"
#include "management.h"
#include "msg_define.h"
#include "plugindef.h"
#include "scm.h"

#include "IvrAuth.h"

#include "voice.h"
#include "plugin.h"
//#include "rtptest.h"
#include "connection.h"
#include "sky_rtp.h"
#include "SystemBaseSet.h"
extern UINT iTotalCallNum;
//static int startPort = 2326;
//,
extern BOOL bRunState;
///extern CRITICAL_SECTION	hFreeCritical;
//extern CRITICAL_SECTION	hMakeCritical;
void WriteLog(char * filename, int a, char * c, char * b);
//#define _SYS_LOG_ENABLE
#ifdef _SYS_LOG_ENABLE
#define WRITELOG(a,b,c,d) WriteLog(a,b,c,d)
#else
#define WRITELOG(a,b,c,d)
#endif

#if 1
#define WRITELOGCM(a,b,c,d) WriteLog(a,b,c,d)
#else
#define WRITELOGCM(a,b,c,d)
#endif

#define RTPCHK_TID		35000
#define RTPCHK_TVAL		600

extern BOOL	m_bRegisteredBISCon;

char *CodecName[]=
{
	"g7231","g711Ulaw64k","g711Alaw64k","g729","none"
};

//#define G729_ENABLE

//#endif //FAST_START

/*
"g711Ulaw"	=1;
"g711Alaw"	=2;
"g7231"		=0;
*/

void RTPChannelOpen(int callNum)
{
	UINT32 rate=64000;
	CONNECTION * pOr=NULL;
	CONNECTION * pConnect = &pCon[callNum];
	if( !pConnect->out[0].hsChan )
	{
		UINT rate = 64000;
#if 0
#if 1
		pConnect->out[0].port = lRtpOpen(pConnect->iCallNumber*10,
			"testChannel", m_InitOfRtpPort);
		cmChannelNew(pConnect->hsCall,
			(HAPPCHAN)&(pConnect->out[0]),
			&(pConnect->out[0].hsChan));
		cmChannelSetRTCPAddress(pConnect->out[0].hsChan,
			0,(UINT16)(pConnect->out[0].port+1));
#else
		int port = lRtpOpen(pConnect->iCallNumber*10,
			"testChannel", m_InitOfRtpPort);
		cmChannelNew(pConnect->hsCall,
			(HAPPCHAN)&(pConnect->out[0]),
			&(pConnect->out[0].hsChan));
		cmChannelSetRTCPAddress(pConnect->out[0].hsChan,
			0,(UINT16)(port+1));
#endif
#endif
		if( callNum >= iMaxAllowedCall )
		{
			pOr = &pCon[callNum-iMaxAllowedCall];
			if( pOr->iChannelCodec == 3 )
			{
				if( !m_g729Support )
					return ;
			}
			sprintf(pConnect->in[0].channelName,"%s", CodecName[pOr->iChannelCodec]);
			sprintf(pConnect->out[0].channelName,"%s", CodecName[pOr->iChannelCodec]);
		}
		else
		{
			sprintf(pConnect->in[0].channelName,"%s", CodecName[0]);
			sprintf(pConnect->out[0].channelName,"%s", CodecName[0]);
		}
		cmChannelOpen(pConnect->out[0].hsChan,pConnect->out[0].channelName,NULL,NULL,rate);
	}
}

void CALLBACK DurationCheckdrop(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{

	KillTimer(m_hWnd, eEventId);
	int iCallNum = eEventId - DURATION_TIMER_ID;
	SendMessage(m_hWnd, WM_VOICON_CallDrop, iCallNum, (long)cmReasonTypeUndefinedReason);
	WRITELOGCM(PROG_NAME, iCallNum, "CDT", "Over Allowed Time.");
}

void CALLBACK RTPOpenCheck(HWND hWnd,UINT uMsg,UINT uId,DWORD dwTime)
{
	int iCallNum = uId - RTPCHK_TID;
	KillTimer(hWnd,uId);
	CONNECTION * pConnect = &pCon[iCallNum];
	if( pConnect )
	{
		if( !pConnect->m_iH245Tunneling && !pConnect->bOpendChannel)
			RTPChannelOpen(iCallNum);
	}
}
#if 0
void WriteLogTest(char * filename, int a, char * c, char * b)
{
	FILE *fp; 
	SYSTEMTIME systemTime;
	char tBuffer[1024];
	memset( tBuffer, 0, 1024);
	char sFileName[256];
	memset(sFileName, 0, 256);
	GetLocalTime(&systemTime);
	sprintf(sFileName, "log/%s-%02d%02d%02d.log", filename, systemTime.wYear, systemTime.wMonth, systemTime.wDay);
	fp = fopen(sFileName, "a+t");
	if(fp)
	{
		sprintf(tBuffer, "[%s : %02d - %02d/%02d/%02d ]%s \r\n", c, a, systemTime.wHour, 
				systemTime.wMinute, systemTime.wSecond, b);
		fwrite(tBuffer, strlen( tBuffer), 1, fp);
		fclose(fp);
	}
}
#endif
/* Set default data */
// 포트 번호를 설정.

UINT SendDTMF(HAPP hApp, int cNum, char *szNum)
{
	SendMessage(m_hWnd, WM_VOICON_SendDTMF, cNum, (LPARAM)szNum[0]);
	return 0;
}

//#ifdef FAST_START
void FillGlobalFSData(int iCallNum)
{
#ifdef M_ATTRIBUTE
	CONNECTION * pOr=NULL;
	if( iCallNum >= iMaxAllowedCall )
		pOr = &pCon[iCallNum-iMaxAllowedCall];
	memset( &pCon[iCallNum].GlobalFSDataRequested, 0, sizeof(cmFastStartMessage));
	pCon[iCallNum].GlobalFSDataRequested.partnerChannelsNum=2;
	
	if( m_g729Support )
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.altChannelNumber=4;
	else
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.altChannelNumber=3;
	
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtp.port= 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtp.ip =  0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].dataTypeHandle = -1;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].rtp.port= 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].rtp.ip =  0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].dataTypeHandle = -1;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].rtp.port= 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].rtp.ip =  0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].dataTypeHandle = -1;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
	if( m_g729Support )
	{
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].rtp.port= 0;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].rtp.ip =  0;
		//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].rtcp.ip = 0;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].dataTypeHandle = -1;
	}
	if( pOr )
	{
		if( pOr->iChannelCodec == 0 )
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].channelName = "g711Ulaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].channelName = "g711Alaw64k";
			if( m_g729Support )
				pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].channelName = "g729";
		}
		else if( pOr->iChannelCodec == 1 )
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g711Ulaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].channelName = "g711Alaw64k";
			if( m_g729Support )
				pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].channelName = "g729";
		}
		else if( pOr->iChannelCodec == 2 )
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g711Alaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].channelName = "g711Ulaw64k";
			if( m_g729Support )
				pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].channelName = "g729";
		}
		else if( pOr->iChannelCodec == 3 && m_g729Support)
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g729";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].channelName = "g711Alaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].channelName = "g711Ulaw64k";
		}
	}
	else
	{
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g7231";
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[1].channelName = "g711Ulaw64k";
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[2].channelName = "g711Alaw64k";
		if( m_g729Support )
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[3].channelName = "g729";
	}
	if( m_g729Support )
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.altChannelNumber=4;
	else
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.altChannelNumber=3;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtp.ip =  0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].dataTypeHandle = -1;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].rtp.ip =  0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].dataTypeHandle = -1;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].rtp.ip =  0;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtp.port= m_InitOfRtpPort+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtp.port= m_InitOfRtpPort+iCallNum*2;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].rtp.port= m_InitOfRtpPort+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].rtp.port= m_InitOfRtpPort+iCallNum*2;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].rtp.port= m_InitOfRtpPort+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].rtp.port= m_InitOfRtpPort+iCallNum*2;
	//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].dataTypeHandle = -1;
	if( m_g729Support )
	{
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].rtp.ip =  0;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].rtcp.ip = 0;
		//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].rtp.port= m_InitOfRtpPort+iCallNum*20;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].rtp.port= m_InitOfRtpPort+iCallNum*2;
		//pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].rtcp.port= m_InitOfRtpPort+1+iCallNum*20;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].dataTypeHandle = -1;
	}
	if( pOr )
	{
		if( pOr->iChannelCodec == 0 )
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].channelName = "g711Ulaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].channelName = "g711Alaw64k";
			if( m_g729Support )
				pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].channelName = "g729";
		}
		else if( pOr->iChannelCodec == 1)
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g711Ulaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].channelName = "g711Alaw64k";
			if( m_g729Support )
				pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].channelName = "g729";
		}
		else if( pOr->iChannelCodec == 2)
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g711Alaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].channelName = "g711Ulaw64k";
			if( m_g729Support )
				pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].channelName = "g729";
		}
		else if( pOr->iChannelCodec == 3 && m_g729Support )
		{
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g729";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].channelName = "g7231";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].channelName = "g711Ulaw64k";
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].channelName = "g711Alaw64k";
		}
	}
	else
	{
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g7231";
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[1].channelName = "g711Ulaw64k";
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[2].channelName = "g711Alaw64k";
		if( m_g729Support )
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[3].channelName = "g729";
	}
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].type = cmCapAudio;
#else
	CONNECTION * pOr=NULL;
	if( iCallNum >= iMaxAllowedCall )
		pOr = &pCon[iCallNum-iMaxAllowedCall];
	memset( &pCon[iCallNum].GlobalFSDataRequested, 0, sizeof(cmFastStartMessage));
	pCon[iCallNum].GlobalFSDataRequested.partnerChannelsNum=2;
	
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.altChannelNumber=1;
	
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtp.port= 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtp.ip =  0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;

	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].dataTypeHandle = -1;
	if( pOr )
	{
		if( pOr->iChannelCodec == 0 )
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g7231";
		else if( pOr->iChannelCodec == 1 )
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g711Ulaw64k";
		else if( pOr->iChannelCodec == 2 )
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g711Alaw64k";
		else if( pOr->iChannelCodec == 3 && m_g729Support)
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g729";
	}
	else
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].transmit.channels[0].channelName = "g7231";
	
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.altChannelNumber=1;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtp.ip =  0;

	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtp.port= m_InitOfRtpPort+iCallNum*2;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtcp.port= m_InitOfRtpPort+1+iCallNum*2;

	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].rtcp.ip = 0;
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].dataTypeHandle = -1;
	if( pOr )
	{
		if( pOr->iChannelCodec == 0 )
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g7231";
		else if( pOr->iChannelCodec == 1)
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g711Ulaw64k";
		else if( pOr->iChannelCodec == 2)
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g711Alaw64k";
		else if( pOr->iChannelCodec == 3 && m_g729Support )
			pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g729";
	}
	else
	{
		pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].receive.channels[0].channelName = "g7231";
	}
	pCon[iCallNum].GlobalFSDataRequested.partnerChannels[0].type = cmCapAudio;
#endif
}

int ForwardCall(char *szPhoneNum, char *szCalling, int iCallNum)
{
	cmCallForward(pCon[iCallNum].hsCall,szPhoneNum);
	return 0;
}

int MakeCall(char *szCallee, char * szCaller, int iCallNum )
{
	int	iLen = 16;
	int i_error = 0;
	int iOutCall = 0;
	iOutCall = iCallNum + iMaxAllowedCall;
	if( pCon[iCallNum].bUsedThisCall)
	{
		if( pCon[iOutCall].hsCall )
			pCon[iOutCall].hsCall = NULL;
		if( cmCallNew(hApp,(HAPPCALL)&pCon[iOutCall],&(pCon[iOutCall].hsCall)) < 0)
		{
			i_error = -1;
		}
		else
		{
			if( pCon[iOutCall].hsCall )
			{
				memset( pCon[iCallNum].des_e164 , 0, 36);
				memcpy( pCon[iCallNum].des_e164, szCallee, 20);
				pCon[iCallNum].iCallState = 3;
				for( int i = 0; i < 36 ; i++)
				{
					if( pCon[iCallNum].des_e164[i] == 0x20)
						pCon[iCallNum].des_e164[i] = 0;
				}
				// Outbound 콜의 정보를 초기화
				pCon[iOutCall].m_iFastStart = m_iFastStart;
				pCon[iOutCall].m_iH245Tunneling = m_iH245Tunneling;
				cmCallSetParam(pCon[iOutCall].hsCall, cmParamCanOverlapSending,0,0,NULL);
				cmCallSetParam(pCon[iOutCall].hsCall, cmParamEarlyH245,        0,1,NULL);
				cmCallSetParam(pCon[iOutCall].hsCall, cmParamH245Tunneling,    0,pCon[iOutCall].m_iH245Tunneling,NULL);
				if( m_iFastStart )
					FillGlobalFSData(iOutCall);
				WriteLogL(PROG_NAME, iCallNum, "OUT", pCon[iCallNum].des_e164);
				strcpy(pCon[iCallNum].src_e164, szCaller);
				pCon[iOutCall].bUsedThisCall = TRUE;
				pCon[iOutCall].bOutIsBusy = 0;
				if( m_iFastStart )
				{
					//lRtpOpen(iOutCall*10, "testChannel", m_InitOfRtpPort);
					pCon[iOutCall].bOpendChannel = TRUE;
				}
				cmCallMake(pCon[iOutCall].hsCall, 64000, 0, pCon[iCallNum].des_e164
					,szCaller,PROG_NAME,"Setup", 5);
				
				int iLen = 18;
				cmCallGetParam(pCon[iOutCall].hsCall,cmParamCID,0,&iLen,(char*)&pCon[iOutCall].cID);

				return 0;
			}
			pCon[iOutCall].bUsedThisCall = TRUE;
			i_error = -1;
		}
	}
	return i_error;
}

void DropCall(int iCallNum, cmReasonType iReason)
{
	if( pCon[iCallNum].hsCall)
	{
		WriteLogL(PROG_NAME, iCallNum, "D-C", "DropCall_0");
		if( pCon[iCallNum].bUsedThisCall )
		{
			//if( iCallNum >= iAllowedMaxCall)
			//{
				WriteLogL(PROG_NAME, iCallNum, "D-C", "DropCall_1");
				//for( int i = 0; i< 10; i++)
				//{
				//	if( pCon[iCallNum].out[i].hsChan)
				//		cmChannelDrop(pCon[iCallNum].out[i].hsChan);
				//}
				if( iReason != 0)
				{
					cmCallDropParam(pCon[iCallNum].hsCall, iReason);
				}
				else
				{
					cmCallDrop(pCon[iCallNum].hsCall);
					//cmCallDropParam(pCon[iCallNum].hsCall, cmReasonTypeNoBandwidth);
				}
			//}
		}
	}
	else
	{
		pCon[iCallNum].bUsedThisCall = FALSE;
		if( iCallNum > iMaxAllowedCall )
		{
			char tbuf[32];
			sprintf(tbuf, "%d / %d / %d", scm[iCallNum-iMaxAllowedCall].m_bCon, 
				pCon[iCallNum-iMaxAllowedCall].bCallReusing,
				pCon[iCallNum-iMaxAllowedCall].bUsedThisCall);
			pCon[iCallNum-iMaxAllowedCall].iCallState = 0;
			WriteLogL(PROG_NAME, iCallNum, "D-C", tbuf);
			if( !pCon[iCallNum-iMaxAllowedCall].bUsedThisCall )
			{
				//if( pCon[iCallNum-iMaxAllowedCall].hsCall )
				//{
				//	cmCallClose(pCon[iCallNum-iMaxAllowedCall].hsCall);
				//	pCon[iCallNum-iMaxAllowedCall].hsCall = NULL;
				//}
				pCon[iCallNum-iMaxAllowedCall].bCallReusing = TRUE;
				pCon[iCallNum-iMaxAllowedCall].iCallState = 0;
			}
		}
	}
}
int getFreeCall()
{
	if( !bRunState )
		return -2;
	//EnterCriticalSection(&hFreeCritical);
	for( int i = 0; i< iMaxAllowedCall; i++)
	{
		if( !pCon[i].bUsedThisCall && !scm[i].m_bCon && pCon[i].bCallReusing)
		{
			pCon[i].bUsedThisCall = TRUE;
			pCon[i].bCallReusing = FALSE;
			//LeaveCriticalSection(&hFreeCritical);
			return i;
		}
	}
	//LeaveCriticalSection(&hFreeCritical);
	return -1;
}

CONNECTION * getConnection(int i)
{
	return &pCon[i];
}

// first enter
int OnEvNewCall(HCALL hsCall, LPHAPPCALL lphaCall)
{
	int freeCall=0;
	freeCall = getFreeCall();
	if( freeCall > -1)
	{
		pCon[freeCall].hsCall = hsCall;
		pCon[freeCall].iCallState = 1;
		pCon[freeCall].bVoiceRec = 0;
		pCon[freeCall].bInChannelConnect = 0;
		pCon[freeCall].bRetrialCall = 0;
		pCon[freeCall].bOutIsBusy = 0;	
		*lphaCall=(HAPPCALL)&pCon[freeCall];
		//cmCallSetParam(hsCall,cmParamCanOverlapSending,0,0,NULL);
		pCon[freeCall].m_iFastStart = m_iFastStart;
		cmCallSetParam(hsCall,cmParamEarlyH245,0,1,NULL);
		// // 2006/10/02 swhors@naver.com
		//cmCallGetParam(hsCall,cmParam

		// 2006/09/06 swhors
		cmCallGetParam(hsCall,cmParamH245Tunneling,0,&pCon[freeCall].m_iH245Tunneling,NULL);
		if( pCon[freeCall].m_iH245Tunneling )
		{
			if( !m_iH245Tunneling )
			{
				pCon[freeCall].m_iH245Tunneling = m_iH245Tunneling;
				cmCallSetParam(hsCall,cmParamH245Tunneling,0,pCon[freeCall].m_iH245Tunneling,NULL);
			}
		}
		memset(&scm[freeCall].billingPacket.pBody, 0x20, sizeof(IVR_AUTHENTICATION_PACKET_BODY)); // IVR_AUTHENTICATION_PACKET
		if( iTotalCallNum < 0xffffffff)
			iTotalCallNum++;
		else
			iTotalCallNum = 0;
		return freeCall;
	}
	if( freeCall == -1 || freeCall == -2)
	{
		*lphaCall=NULL;
		WriteLogL(PROG_NAME, -1, "NEW", "Fail");
	}
	return -1;
}

int OnEvChannelStateChanged(CONNECTION * pConnect, HAPPCHAN haChannel, HCHAN hsChannel, UINT32 state, UINT32 stateMode)
{
	channel * c = (channel *) haChannel;
	HCALL hsCall = pConnect->hsCall;
	if(!haChannel)
	{
		cmChannelClose(hsChannel);
		return ERROR;
	}
	if( !hsChannel)
	{
		return ERROR;
	}
	RvBool origin = FALSE;
	cmChannelGetOrigin(hsChannel,&origin);
	c->state = (int)state;
	c->stateMode = (int) stateMode;
	switch(state)
	{
	case cmChannelStateDisconnected:
		pConnect->bOpendChannel = FALSE;
		if( stateMode == cmChannelStateModeDisconnectedLocal )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedLocal");
		else if( stateMode == cmChannelStateModeDisconnectedRemote )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedRemote");
		else if( stateMode == cmChannelStateModeDisconnectedMasterSlaveConflict )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedMasterSlaveConflict");
		else if( stateMode == cmChannelStateModeDisconnectedReasonUnknown )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedReasonUnknown");
		else if( stateMode == cmChannelStateModeDisconnectedReasonReopen )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedReasonReopen");
		else if( stateMode == cmChannelStateModeDisconnectedReasonReservationFailure )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedReasonReservationFailure");
		break;
	case cmChannelStateConnected:
		{
			if( origin)
			{
				pConnect->bOpendChannel = TRUE;
				
				if( pConnect->iCallNumber < iMaxAllowedCall )
				{
					cmAlias alias;
					alias.type =  cmAliasTypeE164;
					alias.string = pConnect->des_e164;
					alias.length = strlen(pConnect->des_e164);
					int iLen = sizeof(cmAlias);
					cmCallGetParam(pConnect->hsCall, cmParamCalledPartyNumber, 1, &iLen, (char *)&alias);
					iLen = 18;
					//unsigned char tBuf[18];
					cmCallGetParam(pConnect->hsCall,cmParamCID,0,&iLen,(char*)&pConnect->cID);
					wsprintf(scm[pConnect->iCallNumber].billingPacket.pBody.conferenceID,
						"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
						pConnect->cID[0], pConnect->cID[1], pConnect->cID[2], pConnect->cID[3],
						pConnect->cID[4], pConnect->cID[5], pConnect->cID[6], pConnect->cID[7],
						pConnect->cID[8], pConnect->cID[9], pConnect->cID[10], pConnect->cID[11],
						pConnect->cID[12], pConnect->cID[13], pConnect->cID[14], pConnect->cID[15]);
					scm[pConnect->iCallNumber].billingPacket.pBody.conferenceID[32] = 0x20;
					scm[pConnect->iCallNumber].billingPacket.pBody.conferenceID[33] = 0x20;
					iLen = sizeof(cmAlias);
					memset( pConnect->src_e164, 0, 36);
					alias.string = pConnect->src_e164;
					alias.type = cmAliasTypeE164;
					alias.length = 36;
					cmCallGetParam(pConnect->hsCall, cmParamCallingPartyNumber, 0, &iLen, (char *)&alias);
					pConnect->iCallState = 2;
					//cmCallConnectControl( pConnect->hsCall );
					WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "Connect Success");
					SendMessage(m_hWnd, WM_VOICON_CallStart, pConnect->iCallNumber, 0);
				}
				else
				{
					//cmCallConnectControl( pConnect->hsCall );
				}
			}
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateConnected");
		}
		break;
	case cmChannelStateIdle:
		{
			if( pConnect->iCallNumber < iMaxAllowedCall )
			{
				if( pCon[pConnect->iCallNumber+iMaxAllowedCall].bTimerSet )
				{
					KillTimer(m_hWnd, pConnect->iCallNumber + DURATION_TIMER_ID);
					pCon[pConnect->iCallNumber+iMaxAllowedCall].bTimerSet = FALSE;
				}
			}
			//lRtpClose(pConnect->iCallNumber*10+c->sNum);
//			lRtpClose(pConnect->iCallNumber*10);
			reset_rtp_info(pConnect->iCallNumber);
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateIdle");
#ifdef _SYS_LOG_ENABLE
			char Tbuf[128];
			wsprintf(Tbuf,"Close LogicalChannel [%d] : port[%d] state[%d]", origin, c->port, c->state);
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", Tbuf);
#endif
			cmChannelClose(hsChannel);
			c->dpt=0;
			c->hsChan=NULL;
			//c->port= 0;
		}
		break;
	case cmChannelStateOffering:
		WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "Channel State Offering");
		//// Auto Answering Setting
		if (stateMode!=cmChannelStateModeDuplex)
		{
			cmChannelSetAddress(hsChannel,0,(UINT16)c->port);
			cmChannelSetRTCPAddress(hsChannel,0,(UINT16)(c->port+1));
		}
		else
		{
			cmTransportAddress ta={0,0,(UINT16)c->port,cmTransportTypeIP};
			cmChannelSetDuplexAddress(hsChannel,ta,1,"2",FALSE);
		}
		cmChannelSetFlowControlToZero(hsChannel,TRUE);
		cmChannelAnswer(hsChannel);
		if( !pConnect->out[c->sNum].hsChan && !cmChannelIsDuplex(pConnect->in[c->sNum].hsChan))
		{
			UINT rate = 64000;
#if 0
#if 0
			int port = lRtpOpen(pConnect->iCallNumber*10+c->sNum,
										"testChannel", m_InitOfRtpPort);
			if( port > 0 ) pConnect->bOpendChannel=TRUE;
#else
			int port = lRtpOpen(pConnect->iCallNumber*10+c->sNum,
										"testChannel", m_InitOfRtpPort);
			if( port > 0 ) pConnect->bOpendChannel=TRUE;
#endif
#else
			pConnect->bOpendChannel=TRUE;
			//int port = ((pConnect->iCallNumber<iMaxAllowedCall)?m_InitOfRtpPort:(m_InitOfRtpPort+iMaxAllowedCall*2)) + pConnect->iCallNumber*2;
			int port = m_InitOfRtpPort + pConnect->iCallNumber*2;
#endif
			//this->pConnect->bOpendChannel
			cmChannelNew(pConnect->hsCall,
						(HAPPCHAN)&(pConnect->out[c->sNum]),
						&(pConnect->out[c->sNum].hsChan));
			cmChannelSetRTCPAddress(pConnect->out[c->sNum].hsChan,
						0,(UINT16)(port+1));
            strcpy(pConnect->out[c->sNum].channelName,
						pConnect->in[c->sNum].channelName);
			
			if (pConnect->in[c->sNum].dpt)
			{
				cmChannelSetDynamicRTPPayloadType(pConnect->out[c->sNum].hsChan,
						pConnect->in[c->sNum].dpt);
			}
			cmChannelOpenDynamic(pConnect->out[c->sNum].hsChan,
				pvtParent(cmGetValTree(hApp),pConnect->in[c->sNum].handle)
				,NULL/*in[c->sNum].hsChan*/,NULL,FALSE);
        }
		break;
	}
	return 0;
}
#if 0
int OnEvRegEvent( IN cmRegState regState, IN cmRegEvent regEvent, IN int regEventHandle)
{
	if (regState==cmRegistered// || regState==cmResourceAvailabilityConfirmation
		/*|| regState == cmPermanentAlternateGatekeeperConfirmation*/)
	{
		if( 
		m_bRegisteredBISCon = TRUE;
		;
		WriteLog(PROG_NAME, -1, "RCF", "registeredRequest Success");
	}
//	else if( regState == 
	else 
	{
		m_bRegisteredBISCon = FALSE;
		if( regEvent ==  cmUnregistrationRequest)
		{
			if( hApp)
			{
#if 0
				cmUnregister(hApp);
				cmRegister(hApp);
				WriteLogL(PROG_NAME, -2, "URJ", "Receive URQ message.");
#else
				WriteLogL(PROG_NAME, -2, "URQ", "Receive URQ message.");
				SendMessage(m_hWnd, WM_BISCON_REREGISTER, NULL,NULL);
#endif
			}
			else
				WriteLog("error", -2, "URJ", "fail");
		}
		else if (regEvent==cmRegistrationReject)
		{
			char sErrorbuf[120];
			memset(sErrorbuf, 0, 120);
			HPVT hVal=cmGetValTree(hApp);
			int handle=pvtChild(hVal,regEventHandle);
			if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.discoveryRequired")>=0)
				sprintf(sErrorbuf, "rejectReason.discoveryRequired");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidRevision")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidRevision");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidCallSignalAddress")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidCallSignalAddress");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidRASAddress")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidRASAddress");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.duplicateAlias")>=0)
				sprintf(sErrorbuf, "rejectReason.duplicateAlias");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidTerminalType")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidTerminalType");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.undefinedReason")>=0)
				sprintf(sErrorbuf, "rejectReason.undefinedReason");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.transportNotSupported")>=0)
				sprintf(sErrorbuf, "rejectReason.transportNotSupported");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.fullRegistrationRequired")>=0)
				sprintf(sErrorbuf, "rejectReason.fullRegistrationRequired");
			else
				sprintf(sErrorbuf, "rejectReason.UnknownReason");
			WriteLogL(PROG_NAME, -2, "RRJ", sErrorbuf);
			if( hApp )
				cmRegister(hApp);
			//else
			//	WriteLog("error", -2, "RRJ", "null point hApp");
		}
	}
	return 0;
}
#else
int OnEvRegEvent(HAPP hApp,cmRegState regState,cmRegEvent regEvent,int regEventHandle)
{
	if( regEvent == cmRegistrationConfirm )
	{
		m_bRegisteredBISCon = TRUE;
		WriteLog(PROG_NAME, -1, "RCF", "registeredRequest Success");
	}
	else if( regEvent == cmUnregistrationRequest)
	{
		WriteLogL(PROG_NAME, -2, "URQ", "Receive URQ message.");
		SendMessage(m_hWnd, WM_BISCON_REREGISTER, NULL,NULL);
		m_bRegisteredBISCon = FALSE;
	}
	else if (regEvent==cmRegistrationReject)
	{
		char sErrorbuf[120];
		memset(sErrorbuf, 0, 120);
		HPVT hVal=cmGetValTree(hApp);
		int handle=pvtChild(hVal,regEventHandle);
		if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.discoveryRequired")>=0)
			sprintf(sErrorbuf, "rejectReason.discoveryRequired");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidRevision")>=0)
			sprintf(sErrorbuf, "rejectReason.invalidRevision");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidCallSignalAddress")>=0)
			sprintf(sErrorbuf, "rejectReason.invalidCallSignalAddress");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidRASAddress")>=0)
			sprintf(sErrorbuf, "rejectReason.invalidRASAddress");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.duplicateAlias")>=0)
			sprintf(sErrorbuf, "rejectReason.duplicateAlias");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidTerminalType")>=0)
			sprintf(sErrorbuf, "rejectReason.invalidTerminalType");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.undefinedReason")>=0)
			sprintf(sErrorbuf, "rejectReason.undefinedReason");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.transportNotSupported")>=0)
			sprintf(sErrorbuf, "rejectReason.transportNotSupported");
		else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.fullRegistrationRequired")>=0)
			sprintf(sErrorbuf, "rejectReason.fullRegistrationRequired");
		else
			sprintf(sErrorbuf, "rejectReason.UnknownReason");
		WriteLogL(PROG_NAME, -2, "RRJ", sErrorbuf);
		if( hApp )
			cmRegister(hApp);
		//else
		//	WriteLog("error", -2, "RRJ", "null point hApp");
	}
	return 0;
}
#endif

int OnEvCallStateChanged(CONNECTION * pConnect, HCALL _hsCall, UINT32 state, UINT32 stateMode)
{
	int return_code = ERROR;
	int call_tag = 0;
	int ilen = 64;
	int ilen2 = 30;

	if( !pConnect->hsCall )
	{
		cmCallClose(_hsCall);
		return ERROR;
	}
	if( _hsCall != pConnect->hsCall )
	{
		cmCallClose(_hsCall);
		return ERROR;
	}
	HCALL hsCall = pConnect->hsCall;
	RvBool origin=cmCallGetOrigin(_hsCall,NULL);
	if (state!=cmCallStateWaitAddressAck)
		pConnect->overlapState.running=overlapNotInit;
	switch(state)
	{
	case cmCallStateWaitAddressAck:
		{
			cmAlias alias;
			pConnect->overlapState.running=overlapWait;
			pConnect->overlapState.address[0]=0;
			alias.string=pConnect->overlapState.address;
			alias.length=sizeof(pConnect->overlapState.address);
			cmCallGetParam(_hsCall,cmParamCalledPartyNumber,0,0,(char*)&alias);
			alias.length = strlen(alias.string);
		}
		break;
	case cmCallStateConnected:
		{
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "Connected Call");
			// 아래의 루틴은 Q931의 Facility를 전송하는 루틴이다.
			// 이 루틴은 startH245를 전송한다.
			// 2006/07/26 swhors@naver.com
			INT32 value;
			//cmCallGetParam(hsCall, cmParamEstablishH245, 0, &value, NULL);
			cmCallGetParam(hsCall, cmParamH245Tunneling, 0, &pConnect->m_iH245Tunneling, NULL);
			switch(stateMode)
			{
			case cmCallStateModeConnectedCall:
				if( !origin )
				{
//#define RTPCHK_TID		35000
//#define RTPCHK_TVAL		600
					SetTimer(m_hWnd, RTPCHK_TVAL, pConnect->iCallNumber + RTPCHK_TID, RTPOpenCheck); 
					//	RTPChannelOpen(pConnect->iCallNumber);
				}
				else
				{
					if( pConnect->iCallNumber >= iMaxAllowedCall )
					{
						if( pCon[pConnect->iCallNumber-iMaxAllowedCall].bRetrialCall )
						{
							pCon[pConnect->iCallNumber-iMaxAllowedCall].bInChannelConnect = 0;
						}
					}
					// 자기 자신이 건 전화인 경우, 음성 채널이 열리지 않은 경우, 
					// 인바운드 콜의 코덱 정보로 채널을 열어 준다.
					if( !pConnect->m_iH245Tunneling && !pConnect->bOpendChannel)
						RTPChannelOpen(pConnect->iCallNumber);
					// 아웃 바운드가 열린 경우, 최대 통화 시간을 결정해서
					// 타이머를 가동시켜서 일정 시간 후, 호를 강제로 종료 시킨다.
					if( !pConnect->bTimerSet )
					{
						pConnect->bTimerSet = TRUE;
						int iOrCall = pConnect->iCallNumber-iMaxAllowedCall;
						pCon[iOrCall].iCallState = 4;
						char ttbuf[128];
						memset( ttbuf, 0, 128);
						memcpy( ttbuf, scm[iOrCall].billingPacket.pBody.Use_Call_Time, 8);
						int iTime = atoi(ttbuf);
						int iTId = iOrCall+ DURATION_TIMER_ID;
						if( iTime > 30)
							SetTimer(m_hWnd, iTId, iTime*1000-500, DurationCheckdrop);
						else if( iTime < 0 && iTime <=30)
							SetTimer(m_hWnd, iTId, 29700, DurationCheckdrop);
						
						wsprintf(ttbuf, "Set Call Drop Timer, [%d s]", iTime);
						WriteLogL(PROG_NAME, iOrCall, "CDT", ttbuf);
					}
				}
				
				break;
			case cmCallStateModeConnectedControl:
				break;
			case cmCallStateModeConnectedCallSetup:
				{
					if( !pConnect->m_iH245Tunneling )
					{
						{
							cmCallGetParam(hsCall, cmParamEstablishH245, 0, &value, NULL);
							if( !cmCallHasControlSession( hsCall ) )
							{
								int newId;
								HPVT valH = cmGetValTree(hApp);
								int nodeId = cmProtocolCreateMessage(hApp, cmProtocolQ931);
								pvtAdd(valH, nodeId, __q931(callReferenceValue), 0,NULL, NULL);
								pvtAdd(valH, nodeId, __q931(protocolDiscriminator), 8,NULL, NULL);				
								newId = pvtBuildByPath(valH,nodeId, "message.facility", 0,NULL);
								
								int nid1 = pvtAdd(valH, newId, __q931(userUser), 0,NULL, NULL);
								pvtAdd(valH, nid1, __q931(protocolDiscriminator), 5,NULL, NULL);
								
								int newId1 = pvtBuildByPath(valH,nid1,"h323-UserInformation.h323-uu-pdu.h245Tunneling", 0, NULL);
								newId1 = pvtBuildByPath(valH,nid1,"h323-UserInformation.h323-uu-pdu.h323-message-body", 0, NULL);
								//newId1 = pvtAdd(valH, newId1, __q931(facility), 0, NULL, NULL);
								
								int iResult = pvtBuildByPath(valH, newId1, "facility.protocolIdentifier", 6, NULL);
								iResult = pvtBuildByPath(valH, newId1, "facility.reason", 0, NULL);
								iResult = pvtBuildByPath(valH, newId1, "facility.reason.startH245", 0, NULL);
								
								cmCallFacility( hsCall,nodeId );
								//pvtDelete(valH, nid1); /* !! remove temporary cap definition */
							}
						}
					}
				}
				break;
			}
		}
		break;
	case cmCallStateDisconnected:
		{
			switch(stateMode)
			{
			case cmCallStateModeDisconnectedBusy:
				WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedBusy");
				break;
			case cmCallStateModeDisconnectedReject:
				WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedReject");
				break;
			case cmCallStateModeDisconnectedUnreachable:
				WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedUnreachable");
				break;
			case cmCallStateModeDisconnectedNormal:
				WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedNormal");
				break;
			case cmCallStateModeDisconnectedUnknown:
				WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedUnknown");
				break;
			case cmCallStateModeDisconnectedLocal:
				WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedLocal");
				break;
			default:
				WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "Unkown");
				break;
			}
			cmReasonType type;
			cmCallGetParam(_hsCall, cmParamReleaseCompleteReason, 0,(RvInt32*)&type, NULL );
			if( type == cmReasonTypeInConf )
				pConnect->bOutIsBusy = 1;
			if( !pConnect->bOutIsBusy )
			{
				if( pConnect->iCallNumber >= iMaxAllowedCall )
				{
					if( stateMode == cmCallStateModeDisconnectedReject ||
						//stateMode == cmCallStateModeDisconnectedUnreachable ||
						stateMode == cmCallStateModeDisconnectedBusy )
					{
						pConnect->bOutIsBusy = 1;
					}
				}
			}
			pConnect->bUsedThisCall = FALSE;
			if( pConnect->iCallNumber < iMaxAllowedCall)
			{
				pConnect->iCallState = 6;
				memset(&scm[pConnect->iCallNumber].billingPacket.pBody.conferenceID, 0x20, 36);
				SendMessage(m_hWnd, WM_VOICON_CallDrop, (pConnect->iCallNumber + iMaxAllowedCall), (long)cmReasonTypeUndefinedReason);
			}
			else
			{
				if( pCon[pConnect->iCallNumber-iMaxAllowedCall].bInChannelConnect ==0)
				{
					pCon[pConnect->iCallNumber-iMaxAllowedCall].iCallState = 5;
					SendMessage(m_hWnd, WM_VOICON_CallDrop, (pConnect->iCallNumber - iMaxAllowedCall), (long)cmReasonTypeUndefinedReason);
					return 0;
				}
				
				if( pCon[pConnect->iCallNumber-iMaxAllowedCall].bRetrialCall )
				{
					if( !pConnect->bOutIsBusy )
					{
						pCon[pConnect->iCallNumber-iMaxAllowedCall].iCallState = 5;
						SendMessage(m_hWnd, WM_VOICON_CallDrop, (pConnect->iCallNumber - iMaxAllowedCall), (long)cmReasonTypeUndefinedReason);
						return 0;
					}
				}
			}
		}
		break;
	case cmCallStateIdle:
		WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "Idle Call");
		{
			int i = 0;
			//for( i = 0; i < 10 ; i++)
			{
				//lRtpClose(pConnect->iCallNumber*10 + i);
				//lRtpClose(pConnect->iCallNumber*10 + i);
			}
		}
		//if( pConnect->hsCall )
		//	cmCallClose(pConnect->hsCall);
		cmCallClose(_hsCall);
		pConnect->hsCall = NULL;
		pConnect->iCallState = 0;
		pConnect->bCallReusing = TRUE;
		break;
	case cmCallStateOffering:
		WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "Call State Offering");
		cmCallAnswerExt(hsCall,"BISCon IVR","Connect - message",17);
		//if( !origin )
		//	if( !pConnect->m_iH245Tunneling && !pConnect->m_iFastStart )
		//		RTPChannelOpen(pConnect->iCallNumber);
		break;
	case cmCallStateRingBack:
		cmCallGetParam(_hsCall,cmParamH245Tunneling,0,&pConnect->m_iH245Tunneling,NULL);
		break;
	case cmCallStateDialtone:
		{
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "Call State Dialtone");
			if( pConnect->m_iFastStart )
				cmFastStartOpenChannels(_hsCall,&pConnect->GlobalFSDataRequested);
		}
		break;
	}
	return 0;
}
#define __DEBUG__

int OnUserInput(CONNECTION * pConnect, HCALL hsCall, INT32 userInputId)
{
	PCONNECTION pOrCon=NULL;
	char str[7];
	char data[7];
	INT32 dataLength;
	int nodeId;
	int sce_num;
	cmUserInputIndication userInputIndication ;
	cmUserInputSignalStruct userInputSignalStruct;
	cmUserInputData userData={str,sizeof(str)};
	cmNonStandardIdentifier identifier;
	
	if(!pConnect->bUsedThisCall)
	{
		WriteLog("UserInput", pConnect->iCallNumber,"INP", "uncorrect");
		return ERROR;
	}
#ifdef __DEBUG__
	if( pConnect->iCallNumber >= iMaxAllowedCall)
		printf("hello");
#endif
	//cmUserInputSupportIndication userInputSupportIndication;
	memset( data, 0, 7);
	memset( str, 0, 7);
	if( pConnect->iCallNumber < iMaxAllowedCall )
	{
		sce_num = pConnect->iCallNumber;
		pOrCon = pConnect;
	}
	else
	{
		sce_num = pConnect->iCallNumber - iMaxAllowedCall;
		pOrCon = &pCon[sce_num];
	}

	if( !pOrCon )
		return ERROR;
	
	nodeId = cmUserInputGetDetail(hApp,userInputId,&userInputIndication );
	switch(userInputIndication)
	{
	case cmUserInputNonStandard:
		break;
	case cmUserInputAlphanumeric:
		{
			if( !scm[sce_num].bUserInputIgnore )
			{
				scm[sce_num].iStepState = 2;
				scm[sce_num].bUserPressedKey = FALSE;
				dataLength = 7;
				nodeId = cmUserInputGet(hApp,userInputId,&identifier,data,&dataLength,&userData);
				if( !pOrCon->m_iInputDirection )
				{
					if( pConnect->iCallNumber < iMaxAllowedCall)
						strcat(scm[sce_num].szInput, str);
				}
				else 
				{
					if( pConnect->iCallNumber >= iMaxAllowedCall )
						strcat(scm[sce_num].szInput, str);
				}
				if( pConnect->iCallNumber < iMaxAllowedCall)
					SendDTMF(hApp, pConnect->iCallNumber+iMaxAllowedCall, str);
				else
					SendDTMF(hApp, pConnect->iCallNumber-iMaxAllowedCall, str);
			}
		}
		break;
	case cmUserInputSupport:
		//nodeId = cmUserInputSupportGet(hApp,nodeId,&userInputSupportIndication);
		break;
	case cmUserInputSignal:
		{
			if( !scm[sce_num].bUserInputIgnore )
			{
				scm[sce_num].iStepState = 2;
				scm[sce_num].bUserPressedKey = FALSE;
				dataLength = 7;
				nodeId = cmUserInputGetSignal(hApp,nodeId,&userInputSignalStruct);
				wsprintf(str, "%c", userInputSignalStruct.signalType);
				if( !pOrCon->m_iInputDirection )
				{
					if( pConnect->iCallNumber < iMaxAllowedCall)
						strcat(scm[sce_num].szInput, str);
				}
				else 
				{
					if( pConnect->iCallNumber >= iMaxAllowedCall )
						strcat(scm[sce_num].szInput, str);
				}
				if( pConnect->iCallNumber < iMaxAllowedCall)
					SendDTMF(hApp, pConnect->iCallNumber+iMaxAllowedCall, str);
				else
					SendDTMF(hApp, pConnect->iCallNumber-iMaxAllowedCall, str);
			}
		}
		break;
	case cmUserInputSignalUpdate:
		//cmUserInputGetSignalUpdate(hApp,nodeId,&userInputSignalStruct);
		break;
	}
	return 0;
}

int getFreeOutChan(CONNECTION *pConnect)
{
	for(int i=0;i<10;i++)
	{
		if (!pConnect->out[i].hsChan)
			return i;
	}
	return ERROR;
}

int getFreeInChan(CONNECTION *pConnect)
{
	for(int i=0;i<10;i++)
	{
		if (!pConnect->in[i].hsChan)
		{
			return i;
		}
	}
	return ERROR;
}

int OnChannelNew(CONNECTION * pConnect, HCALL _hsCall, HCHAN hsChannel, LPHAPPCHAN lphaChannel)
{
	int sNum = 0;
	HAPPCHAN haBase;
	HCHAN	hsBase;
	if( !pConnect->hsCall)
		return ERROR;
	if( _hsCall != pConnect->hsCall)
		return ERROR;
	if( !hsChannel)
		return ERROR;
	RvBool origin = FALSE;
	cmChannelGetOrigin(hsChannel,&origin);
	if (origin)
	{
		int ret = cmChannelGetDependency(hsChannel, &haBase, &hsBase);
		sNum=getFreeOutChan(pConnect);
		pConnect->out[sNum].hsChan=hsChannel;
		pConnect->out[sNum].sNum = sNum;
		*lphaChannel=(HAPPCHAN)&(pConnect->out[sNum]);
#if 0
#if 0
		pConnect->out[sNum].hsChan=hsChannel;
		pConnect->out[sNum].sNum = sNum;
		*lphaChannel=(HAPPCHAN)&(pConnect->out[sNum]);
		pConnect->out[sNum].port = lRtpOpen(pConnect->iCallNumber*10+sNum,"testChannel",m_InitOfRtpPort);
#else
		pConnect->out[0].hsChan=hsChannel;
		pConnect->out[0].sNum = 0;
		*lphaChannel=(HAPPCHAN)&(pConnect->out[0]);
		pConnect->out[0].port = lRtpOpen(pConnect->iCallNumber*10,"testChannel",m_InitOfRtpPort);
		if( pConnect->out[0].port > 0 ) pConnect->bOpendChannel = TRUE;
#endif
#else
		//pConnect->out[0].port = ((pConnect->iCallNumber<iMaxAllowedCall)?m_InitOfRtpPort:(m_InitOfRtpPort+iMaxAllowedCall*2)) + pConnect->iCallNumber*2;
		pConnect->out[0].port = m_InitOfRtpPort + pConnect->iCallNumber*2;
		pConnect->bOpendChannel = TRUE;
		pConnect->out[0].hsChan=hsChannel;
		pConnect->out[0].sNum = 0;
#endif
		WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "newChannel-out");
	}
	else
	{
		sNum=getFreeInChan(pConnect);
		if (sNum<0)
		{
			*lphaChannel=0;
			cmChannelDrop(hsChannel);
			return 0;
		}
#if 0
#if 0
		pConnect->in[sNum].hsChan=hsChannel;
		pConnect->in[sNum].sNum = sNum;
		*lphaChannel=(HAPPCHAN)&(pConnect->in[sNum]);
		pConnect->in[sNum].port = lRtpOpen(pConnect->iCallNumber*10+sNum,"testChannel",m_InitOfRtpPort);
#else
		pConnect->in[0].hsChan=hsChannel;
		pConnect->in[0].sNum = 0;
		*lphaChannel=(HAPPCHAN)&(pConnect->in[0]);
		pConnect->in[0].port = lRtpOpen(pConnect->iCallNumber*10+0,"testChannel",m_InitOfRtpPort);
		if( pConnect->in[0].port > 0 ) pConnect->bOpendChannel = TRUE;
#endif
#else
		pConnect->in[0].hsChan=hsChannel;
		pConnect->in[0].sNum = 0;
		pConnect->bOpendChannel = TRUE;
//		pConnect->out[0].port = pConnect->iCallNumber*2 + ((pConnect->iCallNumber<iMaxAllowedCall)?m_InitOfRtpPort:(m_InitOfRtpPort+iMaxAllowedCall*2));
		pConnect->out[0].port = pConnect->iCallNumber*2 + m_InitOfRtpPort;
#endif
		WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "newChannel-in");
	}
	return 0;
}

int OnChannelNewRate(CONNECTION * pConnect, channel* c, HCHAN hsChan, UINT32 rate)
{

	RvBool origin=FALSE;
	if( hsChan)
		cmChannelGetOrigin(hsChan,&origin);
	
	if (origin && pConnect->in[c->sNum].hsChan)
		cmChannelFlowControl(pConnect->in[c->sNum].hsChan, rate);
	return 0;
}
