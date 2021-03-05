#include "stdafx.h"

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <rvcommon.h> 

#include <time.h>
#include "rtp.h"
#include "rtcp.h"
#include "rtptest.h"
#include <payload.h>
#include "operate.h"

#include "management.h"
#include "ivrkeydata.h"
#include "voice.h"
#include "scm.h"
#include "SystemBaseSet.h"

#include "connection.h"

#define G723FRAMELENGTH	24
#define G729FRAMELENGTH 20
#define G711FRAMELENGTH	160

int		bEnableRTPChannelRecord;
int		bRtpReArrange;

extern int m_InitOfRtpPort;

#define AUTO_RTCP

//#define _RTP_LOG
int rtpWriteFromFile(CONNECTION &l_pCon, unsigned char *tSendBuf, int dLength,int &iState, int &iReserved);
void WriteLog(char * filename, int a, char * c, char * b);
#if 1
#define WRITELOG(a,b,c) WriteLog("rtp", a, b, c);
#else
#define WRITELOG(a,b,c)
#endif
#pragma pack (push, CMD_ALINMENT, 1)

#define __DEF_USE_ONEWAY__

typedef struct
{
	HRTPSESSION			hRTP;
	HRTCPSESSION		hRTCP;
	UINT32				bytes;
	UINT32				lastTime;
	int					port;
	UINT32				hostIP;
	int					channels;
	int					callNumber;
	int					seq_num;
	//BOOL				bUse;
	DWORD				StartTimeVal;
	BOOL				bClose;
	DWORD				iniTime;
	DWORD				dwLastTimeStamp;
	time_t				startTime;
	BOOL				bRecordRtp;
	FILE				*fp;
	int					rtpSetted;
	int					rtcpSetted;
} RTPCtrl;

#pragma pack (pop, CMD_ALINMENT)

RTPCtrl rC[4001];
static BOOL   multicast;

static UINT32 theTime;

int readg711(char * szFname, char * sbuf);

int max_connection;

typedef struct _voice_buffer
{
	BYTE	voice_data[24];
	int		iCallNum;
}voice_buffer, *pVoice_buffer;

#ifdef __USE_VOICE_MIX__
pVoice_buffer g_voicedata;
#endif

DWORD WINAPI acm_manager(PVOID pArg)
{
	CloseHandle(GetCurrentThread());
	return 0;
}

void SetEnableRTPChannelRecord(BOOL bVal)
{
	bEnableRTPChannelRecord = bVal;
}

void SetRTPRearrange(BOOL bVal)
{
	bRtpReArrange = bVal;
}

void lRtpInit(HWND hWnd, int max_call,UINT32 uIP)
{
	rtpInit();
	rtcpInit();
	max_connection = max_call;
	theTime = 0;
	for( int i = 0; i< 1100; i++)
	{
		rC[i].hRTCP = NULL;
		rC[i].hRTP = NULL;
		rC[i].channels = 0;
		rC[i].callNumber = 0;
		rC[i].bytes = 0;
		rC[i].bClose = FALSE;
		rC[i].seq_num = 0;
		rC[i].iniTime = 0;
		rC[i].StartTimeVal = 0;
		rC[i].dwLastTimeStamp = 0;
		rC[i].bRecordRtp = FALSE;
		rC[i].fp = NULL;
		rC[i].startTime = 0;
		rC[i].rtpSetted = 0;
		rC[i].rtcpSetted = 0;
		rC[i].hostIP = uIP;
		rC[i].port = 0;
	}
}

HRTPSESSION getRTPSession( int iNum)
{
	return rC[iNum].hRTP;
}

HRTCPSESSION getRTCPSession( int iNum)
{
	return rC[iNum].hRTCP;
}

UINT32 lRtpRate(int session)
{
	UINT32 rate=(rC[session].bytes*8)/(GetTickCount()-rC[session].lastTime+1);
	rC[session].lastTime=GetTickCount();
	rC[session].bytes=0;
	return rate;
}

void write_voice_file(int iCallNum, unsigned char * dBuffer, int iSize)
{
	FILE *fp;
	SCM *this_scm = (SCM *)&scm[iCallNum]; // 
	SCENARIO_STEP step;
	if( !this_scm->m_pCon->bVoiceRec)
		return;
	if( GetScenarioItem(this_scm->iCurPage, this_scm->iCurStep, step) )
	{
		fp = fopen(step.sub_item.function.dll_name, "a+b");
		if( fp )
		{
			fwrite(dBuffer, 1, iSize, fp);
			fclose(fp);
		}
	}
}

//void WriteLogTest(char * filename, int a, char * c, char * b);

#define MAXTestChan 2
//,NOTHREADS
static int g_seq = 0;
/***********************************************************************\
                        End of Special ATM part
\***********************************************************************/
void eventHandler(HRTPSESSION hRTP, void* context)
{
	char buff[256];
	RTPCtrl*  rc = (RTPCtrl*)context;
	rtpParam p;
	int rlen=0;
	if( !hRTP || !rc )
	{
		WriteLog("RTP", -1, "EVT", "error");
#ifdef _RTP_LOG
		char tbuf[128];
		wsprintf(tbuf, "read error");
		WRITELOG(-1, "event", tbuf);
#endif
		return;
	}
	//memset(buff, 0, 256);
	//memset(&p, 0, sizeof(rtpParam));
	
#ifndef AUTO_RTCP
	if( (rlen=rtpRead(hRTP,buff,256,&p)) < 0 )
		return;
#else
	//if( (rlen=rtpRead(hRTP,buff,256,&p)) < 0 )
	if( rtpReadEx(hRTP,buff,256,(GetCurrentTime()-theTime)*8,&p) < 0)
		return;
#endif
#ifndef AUTO_RTCP
	if( rc->hRTCP )
		rtcpRTPPacketRecv(rc->hRTCP,p.sSrc,(GetCurrentTime()-theTime)*8,p.timestamp,p.sequenceNumber);
#endif
	{
		//int wsession = ((rc->callNumber)>=max_connection*10)?((rc->callNumber)-max_connection*10):((rc->callNumber)+max_connection*10);
		int wsession=0;
		int iCallNum = rc->callNumber/10;
		if( iCallNum>=max_connection )
			wsession = iCallNum*10-max_connection*10;
		else
			wsession = iCallNum*10+max_connection*10;

#ifdef _RTP_LOG
		char tbuf[128];
		wsprintf(tbuf, "read from %d : %d", rc->callNumber, wsession);
		WRITELOG(rc->callNumber, "event", tbuf);
#endif
		if( rC[wsession].hRTP && pCon[wsession/10].bOpendChannel )//&& pCon[rsession/10].bOpendChannel)
		{
			int iInterval;
			if( bRtpReArrange )
			{
				if(p.payload==G729)
				{
					iInterval = 200;
				}
				else if( p.payload == PCMU || p.payload == PCMA)
				{
					iInterval = 120;
				}
				else 
				{
					p.sByte = 12;
					p.len = 36;
					iInterval = 240;
				}
				if(!rc->dwLastTimeStamp )
				{
					p.marker = 1;
				}
				else
				{
					int iTimeMux = (p.timestamp - rc->dwLastTimeStamp)/iInterval;
					if( iTimeMux <= 1)
					{
						p.marker = 0;
						iTimeMux = 1;
					}
					else
						p.marker = 1;
					iInterval = iTimeMux * iInterval;
				}
				rc->dwLastTimeStamp = p.timestamp;
				rC[wsession].iniTime += iInterval;
				
				p.timestamp = rC[wsession].iniTime;
				p.sequenceNumber = rC[wsession].seq_num++;
				//WriteLog("RTP", rc->callNumber/10, "EVT", "out");
				//for( int i = 0; i < 10 ; i++ )
				//{
				//	if( rC[wsession+i].hRTP )
						rtpWrite(rC[wsession].hRTP,&buff,rlen,&p);
				//}
			}
			else
			{
				//for( int i = 0; i < 10 ; i++ )
				//{
					//p.payload = G7231;
					//p.marker = 1;
					//p.sByte = 12;
					//p.len = 36;
					//if( rC[wsession].hRTP )
						rtpWrite(rC[wsession].hRTP,&buff,32,&p);
					//else
					//{
					//	char tbuf[128];
					//	wsprintf(tbuf, "read ERROR from %d : %d", rc->callNumber, wsession);
					//	WRITELOG(rc->callNumber, "event", tbuf);
					//}
			}
			
			//rtpWrite(rC[wsession].hRTP,&buff,12 + G723FRAMELENGTH,&p);
			if( bEnableRTPChannelRecord )
			{
				int iMax;
				if( rc->fp ) 
				{
					if( p.payload == G7231 )
						iMax = 24;
					else if(p.payload==G729)
					{
						iMax = 200;
					}
					else if( p.payload == PCMU || p.payload == PCMA)
					{
						iMax = 120;
					}

					if( iMax == p.len-12 )
					{
						time_t t;
						time(&t);
						fwrite(&t, sizeof(time_t), 1, rc->fp);
						fwrite(buff+12, 1, p.len-12, rc->fp);
					}
				}
			}
			
#ifdef _RTP_LOG
			char tbuf[128];
			wsprintf(tbuf, "write to %d", wsession);
			WRITELOG(rc->callNumber, "event", tbuf);
#endif
		}
#ifndef AUTO_RTCP
		if( rC[wsession].hRTP && pCon[wsession/10].bOpendChannel && pCon[rsession/10].bOpendChannel)
		{
			if( rc->hRTCP )
				rtcpRTPPacketSent(rc->hRTCP,p.len,p.timestamp);
		}
#endif
	}
}
#define AUTO_RTCP

int lRtpOpen(int session, char* name, int startPort)
{
	char cName[128];
	rC[session].callNumber = session;
	rC[session].channels++;
	lRtpRate(session);
	
	//char buff[256];
	//memset( buff, 0 , 256 );
	//wsprintf( buff, "cha:%d port:%d", rC[session].channels, startPort );
	//WriteLogTest("RTP", session, buff , "lRtpOpen");
	
	if (rC[session].hRTP)
	{
		return rtpGetPort(rC[session].hRTP);
	}
	
	wsprintf(cName, "%s-%d", name, session);
	rC[session].iniTime = GetTickCount();
	rC[session].StartTimeVal = rC[session].iniTime;
	rC[session].seq_num = 0;
	rC[session].bClose = FALSE;
	rC[session].dwLastTimeStamp = 0;
	rC[session].bRecordRtp = FALSE;
	time(&rC[session].startTime);
	rC[session].hRTP=rtpOpenEx
			( (UINT16)(session*2+startPort),1,0xff, 
#ifndef AUTO_RTCP
			NULL
#else
			cName
#endif
			);
	rtpUseSequenceNumber(rC[session].hRTP);
	if (!rC[session].hRTCP)
#ifndef AUTO_RTCP
		rC[session].hRTCP = rtcpOpen(rtpGetSSRC(rC[session].hRTP),
			(UINT16)(session * 2 + startPort + 1), cName);
#else
		rC[session].hRTCP = rtpGetRTCPSession(rC[session].hRTP);
#endif
	rtpSetEventHandler(rC[session].hRTP,eventHandler,NULL/*&rC[session]*/);
#ifdef _RTP_LOG
	char tbuf[128];
	wsprintf(tbuf, " open rtp session : %d", session);
	WRITELOG(session, "open", tbuf);
#endif
	if( bEnableRTPChannelRecord )
	{
		if( session/10 >= max_connection )
		{
			char fName[256], ext[5];
			int iOrChannel = session/10;
			int iParentChannel = (session/10)-max_connection;
			
			if( pCon[iParentChannel].iChannelCodec == 0)
				wsprintf(ext,"g7231");
			else if( pCon[iParentChannel].iChannelCodec == 3 )
				wsprintf(ext,"g729");
			else
				wsprintf(ext,"g711");
			wsprintf(fName, "./voice/%ld%03d.%s", rC[iParentChannel*10].startTime, iParentChannel,ext);
			rC[iParentChannel*10].fp = fopen(fName, "a+b");
			
			wsprintf(fName, "./voice/%ld%03d.%s", rC[iParentChannel*10].startTime, session/10,ext);
			rC[session].fp = fopen(fName, "a+b");
		}
		else
		{
			rC[session].fp = NULL;
		}
	}
	return session*2+startPort;
}



void lRtpSetRtp(int session,UINT32 ip,UINT16 port)
{
	if( rC[session].hRTP )
	{
		if( !rC[session].rtpSetted && port > 0 )
		{
			rC[session].rtpSetted = 1;
			rtpSetRemoteAddress(rC[session].hRTP,ip,port);
			if (multicast) 
				rtpSetGroupAddress(rC[session].hRTP,ip);
			rtpSetEventHandler(rC[session].hRTP,eventHandler,&rC[session]);
		}
	}
}

void lRtpSetRtcp(int session, UINT32 ip,UINT16 port)
{
	if (rC[session].hRTCP)
	{
		if( !rC[session].rtcpSetted && port > 0 )
		{
			rC[session].rtcpSetted = 1;
			rtcpSetRemoteAddress(rC[session].hRTCP,ip,port);
			if (multicast)
				rtcpSetGroupAddress(rC[session].hRTCP,ip);
		}
	}
}

int lRtpClose(int session)
{
	if (!rC[session].channels) return 0;
	rC[session].channels--;
	if (!rC[session].channels)
	{
		if( rC[session].hRTP)
		{
#ifdef _RTP_LOG
			char tbuf[128];
			wsprintf(tbuf, "rtpclose : %d", session);
			WRITELOG(session, "close", tbuf);
#endif
			rtpClose(rC[session].hRTP);
			rC[session].hRTP=NULL;
#ifndef AUTO_RTCP
			rtcpClose(rC[session].hRTCP);
#endif
			rC[session].hRTCP=NULL;
			rC[session].rtpSetted = 0;
		}
	}
	if( bEnableRTPChannelRecord )
	{
		if( rC[session].fp )
			fclose(rC[session].fp);
		rC[session].fp = NULL;
	}

	return 0;
}

void lRtpEnd(void)
{
	rtcpEnd();
	rtpEnd();
	//if( rC ){
		//free( rC) ;
	//	rC = NULL;
	//}
}

RvBool RVCALLCONV enumerator(IN HRTCPSESSION hRTCP, IN UINT32  ssrc)
{
	char line[100];
	RTCPINFO info;
	rtcpGetSourceInfo(hRTCP, ssrc, &info);
	sprintf(line,"SSRC=%x SEQ=%d LOST=%d CNAME=%s",ssrc, info.rrFrom.sequenceNumber, info.rrFrom.cumulativeLost, info.cname);
	return FALSE;
}

void lRtpTimer(int param)
{
	//static buff[812];
	//static int ii=12345678;
	//if (param==2)
	//{
	//	static j;
	//	j++;
		//for (int i=0;i<100;i++)
		//	if (rC[i].hRTCP)
		//	{
		//		rtcpEnumParticipants(rC[i].hRTCP,enumerator);
		//	}
	//}
}


void rtpWriteRTP(BOOL write)
{
}

int RTPPlayG711File(int iCodec, CONNECTION &pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved);
int RTPPlayG723File(int iCodec, CONNECTION &pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved);
int RTPPlayG729File(int iCodec, CONNECTION &pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved);
int RtpPlay(int iCodec, 
			CONNECTION &pCon, 
			char * szFileName, 
			char * sLanguage, 
			int & iState,
			int iSkipState, 
			int &iReserved)
{
#if 1
	//WriteLog("rtp", iCodec, "RtpPlay", "IN");

	if( pCon.iChannelCodec == 1 || pCon.iChannelCodec == 2)
	{
		//WriteLog("rtp", iCodec, "RtpPlay", "play711");
		return RTPPlayG711File(iCodec, pCon, szFileName, sLanguage, iState, iSkipState, iReserved);
		//return RTPPlayG711File(&pCon, pCon.iCallNumber, sMentFile);
	}
	else if( pCon.iChannelCodec == 0)
	{
		//WriteLog("rtp", iCodec, "RtpPlay", "play7231");
		return RTPPlayG723File(iCodec, pCon, szFileName, sLanguage, iState, iSkipState, iReserved);
	}else if( pCon.iChannelCodec == 3 || pCon.iChannelCodec == 4 )
	{
		//WriteLog("rtp", iCodec, "RtpPlay", "play729");
		return RTPPlayG729File(iCodec, pCon, szFileName, sLanguage, iState, iSkipState, iReserved );
	}
	return -1;
#else
	return 0;
#endif 
}

int RTPPlayG711File(int iCodec, CONNECTION &l_pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved)
{
	int iSession = l_pCon.iCallNumber;
	int		nLen = 0;
	//unsigned char	tSendBuf[MAX_MENT_BUFF_SIZE];
	unsigned char	tPacketBuffer[174];
	unsigned char   *pSendBuf;
	int l_seq = 0;
	rtpParam	p;
	int	rtp_return = 0, sleep_timer = 0;
	int iFileLanguagePage = -1;
	int iFileCodecPage = -1;
	int iFileNum = -1;
	//memset(tSendBuf, 0, MAX_MENT_BUFF_SIZE);
	//nLen = GetMentDataLength(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, tSendBuf);
	nLen = GetMentData(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, pSendBuf);
	if( nLen <= 0)
		return -1;
	int readlen=12;
	p.payload=PCMU;
	p.marker = 1;
	p.sByte = 12;
	//int tLen = 0;
	//DWORD ini_time = GetTickCount();

	//p.timestamp = ini_time;
	p.timestamp = rC[iSession*10].iniTime;

	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	
	while( readlen < nLen)
	{
		if( rC[iSession].hRTP)
		{
			//rC[iSession].iFirstMark = 0;
			if( rC[iSession].bClose != TRUE)
			{
				memset(tPacketBuffer, 0, 174);
				//memcpy( (char*)&tPacketBuffer[12], (char *)&tSendBuf[readlen], G711FRAMELENGTH);
				memcpy( (char*)&tPacketBuffer[12], (char *)&pSendBuf[readlen], G711FRAMELENGTH);

				//rC[iSession].bUse = TRUE;
				if( iState )
					goto endloop;
				if( l_pCon.bOpendChannel )
				{
					p.sequenceNumber = rC[iSession*10].seq_num ++;
					p.timestamp += 120;
					rC[iSession*10].iniTime += 120;
					p.payload=PCMU;
					p.sByte = 12;
					if(l_pCon.bOpendChannel)
						rtp_return = rtpWrite(rC[iSession*10].hRTP,tPacketBuffer,G711FRAMELENGTH+12,&p);
					else
						goto endloop;
				}
				else
				{
					//rC[iSession].bUse = FALSE;
					goto endloop;
				}
				//rC[iSession].bUse = FALSE;
			}
			else
			{
				goto endloop;
			}
		}
		readlen += G711FRAMELENGTH;
		if( p.marker == 1)
		{
			p.marker = 0;
		}
		if( hEvent )
			WaitForSingleObject(hEvent, 15);
		else
			Sleep( 15 );
	}
	if( hEvent )
		WaitForSingleObject(hEvent, 300);
	else
		Sleep(300);
	if( hEvent )
		CloseHandle(hEvent);
	return 0;
endloop:
	if( hEvent )
		CloseHandle(hEvent);
	return 1;
}

int rtpWriteFromFile(CONNECTION &l_pCon, unsigned char *tSendBuf, int dLength,int &iState, int &iReserved)
{
	int iSkip_count=0;
	rtpParam	p;
	int readlen=0;
	int iSession = l_pCon.iCallNumber;
	unsigned char	tPacketBuffer[37];
	int	rtp_return = 0;
	p.payload=G7231;
	p.marker = 1;
	p.sByte	 = 12;
	//rC[iSession*10].iniTime = GetTickCount();
	//DWORD ini_time = GetTickCount();
	//p.timestamp = ini_time;
	p.timestamp = rC[iSession*10].iniTime;
	readlen+=4;

	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	while( readlen < dLength )
	{
		if( !l_pCon.bOpendChannel )
		{
			if( hEvent )
				CloseHandle(hEvent);
			return 1;
		}
		if( iSkip_count == 10)
		{
			if( iState == 2)
			{
				// current ment stop
				if( hEvent )
					CloseHandle(hEvent);
				return 0;
				//goto end_normal;
			}
			else
			{
				iReserved = FALSE;
			}
		}
		else
		{
			iSkip_count ++;
		}
		if( iState == 1)
		{ // current step end
			//if( bConvert == TRUE)
			//	pCon[l_pCon.iCallNumber+iMaxAllowedCall].bOpendChannel = TRUE;
			if( hEvent )
				CloseHandle(hEvent);
			return -1;
		}
		if( iState == 3)
		{
			if( hEvent )
				CloseHandle(hEvent);
			return 0;
		}
		if( rC[iSession*10].hRTP)
		{
			if( rC[iSession*10].bClose != TRUE)
			{
				//rC[iSession].iFirstMark = 0;
				memset(tPacketBuffer, 0, 37);
				int wLen = 0;
				p.sByte	 = 12;
				p.payload=G7231;
				RvInt32 rtpLen = 12 + G723FRAMELENGTH;
				memcpy( (char*)&tPacketBuffer[12], (char *)&tSendBuf[readlen], G723FRAMELENGTH);
				if( l_pCon.bOpendChannel && l_pCon.bUsedThisCall)
				{
					p.sequenceNumber = rC[iSession*10].seq_num++;
					p.timestamp += 240;
					rC[iSession*10].iniTime += 240;
					if(l_pCon.bOpendChannel)
					{
						if( rC[iSession*10].hRTP )
							rtp_return = rtpWrite(rC[iSession*10].hRTP,tPacketBuffer,rtpLen,&p);
					}
#ifndef AUTO_RTCP
					if(l_pCon.bOpendChannel)
					{
						if( rC[iSession*10].hRTCP )
							rtcpRTPPacketSent(rC[iSession*10].hRTCP,p.len,p.timestamp);
					}
#endif
				}
			}
			else
			{
				// goto end
				if( hEvent )
					CloseHandle(hEvent);
				return 1;
			}
		}
		readlen += G723FRAMELENGTH;
		if( p.marker == 1)
		{
			p.marker = 0;
		}
		if( hEvent )
			WaitForSingleObject(hEvent,29);
		else
			Sleep(29);
	}
	if( hEvent )
		CloseHandle(hEvent);
	return 1;
}

int RTPPlayG723File(int iCodec, CONNECTION &l_pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved)
{
	int		nLen = 0;
	//unsigned char	tSendBuf[MAX_MENT_BUFF_SIZE];
	unsigned char	*pSendBuf=NULL;
	int	rtp_return = 0;
	int iFileLanguagePage = -1;
	int iFileCodecPage = -1;
	int iFileNum = -1;
	//memset(tSendBuf, 0, MAX_MENT_BUFF_SIZE);
	//nLen = GetMentDataLength(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, tSendBuf);
	nLen = GetMentData(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, pSendBuf);
	if( nLen <= 0 || !pSendBuf)
		return -1;
	
	//rtp_return = rtpWriteFromFile(l_pCon, tSendBuf, nLen, iState,iReserved);
	rtp_return = rtpWriteFromFile(l_pCon, pSendBuf, nLen, iState,iReserved);
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if( hEvent)
	{
		//append
		WaitForSingleObject(hEvent, 300);
		CloseHandle(hEvent);
	}
	else
		Sleep(300);
	return rtp_return;
}


int rtpWriteFromFile729(CONNECTION &l_pCon, unsigned char *tSendBuf, int dLength,int &iState, int &iReserved)
{
	int iSkip_count=0;
	rtpParam	p;
	int readlen=0;
	int iSession = l_pCon.iCallNumber;
	unsigned char	tPacketBuffer[33];
	int	rtp_return = 0;		
	p.payload=G729;
	p.marker = 1;
	p.sByte	 = 12;
	//DWORD ini_time = GetTickCount();

//	p.timestamp = ini_time;
	//p.timestamp = 0 ;
	p.timestamp = rC[iSession*10].iniTime;
//	readlen+=4;

//	readlen+=4;
	int	time_count = dLength / G729FRAMELENGTH;
	int time_13count = time_count / 3 ;
	int time_23count = ( time_count * 2 ) / 3 ;
	int op_count = 0;
	wsprintf((char*)&tPacketBuffer,"RTP-%03d", l_pCon.iCallNumber);
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,(char*)&tPacketBuffer);
	
	while( readlen < dLength )
	{
		if( !l_pCon.bOpendChannel )
		{
			if( hEvent )
				CloseHandle(hEvent);
			return 1;
		}
		if( iSkip_count == 10)
		{
			if( iState == 2)
			{
				if( hEvent )
					CloseHandle(hEvent);
				// current ment stop
				return 0;
				//goto end_normal;
			}
			else
			{
				iReserved = FALSE;
			}
		}
		else
		{
			iSkip_count ++;
		}
		if( iState == 1)
		{ // current step end
			//if( bConvert == TRUE)
			//	pCon[l_pCon.iCallNumber+iMaxAllowedCall].bOpendChannel = TRUE;
			if( hEvent )
				CloseHandle(hEvent);
			return -1;
		}
		if( iState == 3)
		{
			if( hEvent )
				CloseHandle(hEvent);
			return 0;
		}
		if( rC[iSession*10].hRTP)
		{
			if( rC[iSession*10].bClose != TRUE)
			{
				//rC[iSession].iFirstMark = 0;
				memset(tPacketBuffer, 0, 33);
				int wLen = 0;
				//if( (readlen - G723FRAMELENGTH) > G723FRAMELENGTH )
				//{
				//}
				memcpy( (char*)&tPacketBuffer[12], (char *)&tSendBuf[readlen], G729FRAMELENGTH);
				if( l_pCon.bOpendChannel && l_pCon.bUsedThisCall)
				{
					p.sequenceNumber = rC[iSession*10].seq_num++;
					p.timestamp += 200;
					rC[iSession*10].iniTime += 200;
					p.payload=G729;
					p.sByte	 = 12;
//					p.timestamp += 79;
					if(l_pCon.bOpendChannel)
					{
						if( rC[iSession*10].hRTP )
							rtp_return = rtpWrite(rC[iSession*10].hRTP,tPacketBuffer,G729FRAMELENGTH + 12,&p);
					} 
#ifndef AUTO_RTCP
					if(l_pCon.bOpendChannel)
					{
						if( rC[iSession*10].hRTCP )
							rtcpRTPPacketSent(rC[iSession*10].hRTCP,p.len,p.timestamp);
					}
#endif
				}
			}
			else
			{
				// goto end
				if( hEvent )
					CloseHandle(hEvent);
				return 1;
			}
		}
		readlen += G729FRAMELENGTH;
		if( p.marker == 1)
		{
			p.marker = 0;
		}


		op_count++;
/*
		if( op_count < time_13count )
			Sleep(15);
		else if( op_count < time_23count )
			Sleep(16);
		else 
			Sleep(15);
*/
		if( hEvent )
		{
			if( (op_count %3 ) == 0)
				WaitForSingleObject(hEvent,16);
			else
				WaitForSingleObject(hEvent,15);
		}
		else
		{
			if( ( op_count % 3 ) == 0 )
				Sleep(16);
			else
				Sleep(15);
		}
//		Sleep(8);
	}
	if( hEvent )
		CloseHandle(hEvent);

	return 1;
}


int RTPPlayG729File(int iCodec, CONNECTION &l_pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved)
{
	int		nLen = 0;
	//unsigned char	tSendBuf[MAX_MENT_BUFF_SIZE];
	unsigned char	*pSendBuf;
	int	rtp_return = 0;
	int iFileLanguagePage = -1;
	int iFileCodecPage = -1;
	int iFileNum = -1;
	//memset(tSendBuf, 0, MAX_MENT_BUFF_SIZE);
	//nLen = GetMentDataLength(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, tSendBuf);
	nLen = GetMentData(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, pSendBuf);	
	//WriteLog("rtp", nLen, "FileName:", szFile);

	if( nLen <= 0)
		return -1;
	//WriteLog("RTP", l_pCon.iCallNumber , "CDC", "play 729");

	rtp_return = rtpWriteFromFile729(l_pCon, pSendBuf, nLen, iState,iReserved);
	//append
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if( hEvent )
	{
		WaitForSingleObject(hEvent, 300);
		CloseHandle(hEvent);
	}
	else
		Sleep(300);
	return rtp_return;
}