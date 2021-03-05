#include "stdafx.h"
#include <stdio.h>
#include <time.h>

#include "ivrkeydata.h"
#include "scm.h"
#include "voice.h"
#include "connection.h"
#include "management.h"
#include "SystemBaseSet.h"

#include "sky_lock.h"
#include "sky_thread.h"
#include "sky_sock.h"

#include "sky_rtp.h"

static int rtp_num;

static int rtp_start_port;

#define G723FRAMELENGTH	24
#define G729FRAMELENGTH 20
#define G711FRAMELENGTH	160

//#define USE_INRTCP_REPLY
//#define USE_OUTRTCP_RELAY

int		bEnableRTPChannelRecord;

SKYHANDLE h_inRTPMANAGER;
SKYHANDLE h_outRTPMANAGER;
SKYHANDLE h_inRTCPMANAGER;
SKYHANDLE h_outRTCPMANAGER;
SKY_LOCK  mutex_rtp_inf=SKY_MUTEX_INI;

#define RTP_DEAD_TIME    7000

static int rtp_manager_flag;

int RTPMAX_USER;

static p_rtp_proxy_buf_t pRtpProxyBuf;

void SetEnableRTPChannelRecord(BOOL bVal)
{
	bEnableRTPChannelRecord = bVal;
}

void rtp_manager_lock()
{
    sky_mutex_lock(&mutex_rtp_inf);
}

void rtp_manager_unlock()
{
    sky_mutex_unlock(&mutex_rtp_inf);
}

void enable_rtp_manager_flag()
{
    rtp_manager_flag = 1;
}

void disable_rtp_manager_flag()
{
    rtp_manager_flag = 0;
}

int uncapRTPHeader(IN unsigned char * buff,IN int len,OUT rtp_hdr_t *p)
{
	unsigned char *pbuf;
	pbuf = (unsigned char *)p;
	if( len != 12 )
		return 0;
	memset(p, 0, sizeof(*p));
	for(int i = 0; i< len/4 ;i++)
	{
		*(pbuf+i*4+3) = buff[i*4];
		*(pbuf+i*4+2) = buff[i*4+1];
		*(pbuf+i*4+1) = buff[i*4+2];
		*(pbuf+i*4  ) = buff[i*4+3];
	}
	return 1;
}

int capRTPHeader(rtp_hdr_t p, unsigned char * buff,int len)
{
	unsigned char *pbuf;
	pbuf = (unsigned char*)&p;
	if(len <12 )
		return 0;
	for(int i=0; i < len/4;i++)
	{
		buff[i*4] = (char)*(pbuf+i*4+3);
		buff[i*4+1] = (char)*(pbuf+i*4+2);
		buff[i*4+2] = (char)*(pbuf+i*4+1);
		buff[i*4+3] = (char)*(pbuf+i*4);
	}
	return 1;
}

long make_rtp_stamp(char * se164, char *de164)
{
    long d_e164, s_e164;
	if( se164 )
        s_e164 = atol(se164);
	else
		s_e164 = 0;
	if( de164 )
		d_e164 = atol(de164);
	else
		d_e164 = 0;
    return d_e164+s_e164;
}



void reset_rtp_buf_basic(p_rtp_proxy_buf_t pbuff)
{
    memset(&pbuff->indesip, 0, 24);
    memset(&pbuff->outdesip, 0, 24);
    pbuff->outdesport = 0;
    pbuff->indesport = 0;
    pbuff->rtpStamp = 0;
    pbuff->rtpTime = 0;    
	pbuff->inssrc = 0;
	pbuff->outssrc = 0;
	pbuff->seq = 0;
	pbuff->infp = NULL;
	pbuff->outfp = NULL;
}

void set_ssrc(rtp_hdr_t &hd, unsigned int ssrc)
{
	hd.ssrc		=	ssrc;
}

void set_mark(rtp_hdr_t &hd, BOOL mark)
{
	hd.m = mark;
}

int build_rtpPack(rtp_hdr_t& hd, unsigned int timeStamp, short sequenceNum,BOOL mark, unsigned int ssrc)
{
	memset(&hd, 0, sizeof(rtp_hdr_t));
	hd.version	=	2;
	//hd.p = 0;
	//hd.cc = 
	hd.m		=	mark;
	hd.seq		=	sequenceNum;
	hd.ts		=	timeStamp;
	hd.ssrc		=	ssrc;
	return 0;
}

//
	/* iCodecType 
		0 - G7231/64
		1 - G7231/53
		2 - G711Ulaw
		3 - G711Alaw
	*/
//

int rtpWriteFromFile(int iCodec, CONNECTION &l_pCon, unsigned char *tSendBuf, int dLength,int &iState, int &iReserved)
{
	int iSkip_count=0;
	rtp_hdr_t	p;
	int readlen=0;
	int iSession = l_pCon.iCallNumber;
	int iReadLen, SleepTime,iTimeStamp;
	unsigned char	tPacketBuffer[37];
	int	rtp_return = 0;
	memset(&p, 0, 12);
	switch(iCodec )
	{
	case 4: // G729
		p.pt = G729;
		iReadLen = G729FRAMELENGTH;
		SleepTime = 16;
		iTimeStamp = 200;
	case 2: // G711Ulaw
		p.pt = PCMU;
		iReadLen = G711FRAMELENGTH;
		SleepTime = 300;
		iTimeStamp = 120;
		break;
	case 3:// G711Alaw
		p.pt = PCMA;
		iReadLen = G711FRAMELENGTH;
		SleepTime = 300;
		iTimeStamp = 120;
		break;
	case 0: //g7231 - 63
	case 1: // g7231 - 53
	default:
		p.pt=G7231;
		iReadLen = G723FRAMELENGTH;
		SleepTime = 29;
		iTimeStamp = 240;
		break;
	}
	p.m = 1;
	DWORD ini_time = GetTickCount();
	p.ts = ini_time;
	p.version = 2;
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
		{
			// current step end
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
		int iSendNum = 0;
		int sendHandle;
				
		if( l_pCon.iCallNumber >= rtp_num )
		{
			iSendNum = l_pCon.iCallNumber - rtp_num;
			sendHandle = pRtpProxyBuf[iSendNum].outRtpHandle;
		}
		else
		{
			iSendNum = l_pCon.iCallNumber;
			sendHandle = pRtpProxyBuf[iSendNum].inRtpHandle;
		}
		
		if( sendHandle )
		{
			unsigned char rtpHd[13];
			int sLen = 12 + iReadLen;
			p.ts += iTimeStamp;
			p.seq =pRtpProxyBuf[iSendNum].seq++;
			memset(rtpHd, 0, 13);
			if( iSendNum > rtp_num )
				p.ssrc = pRtpProxyBuf[iSendNum].outssrc;
			else
				p.ssrc = pRtpProxyBuf[iSendNum].inssrc;
			capRTPHeader(p, rtpHd, 12);
			
			memcpy( (char*)&tPacketBuffer, rtpHd, 12);
			memcpy( (char*)&tPacketBuffer[12], (char *)&tSendBuf[readlen], iReadLen);
			if( l_pCon.iCallNumber >= rtp_num )
			{
                sendto_sock(sendHandle, pRtpProxyBuf[iSendNum].outdesip, pRtpProxyBuf[iSendNum].outdesport, (char*)&tPacketBuffer[0], sLen);
                time(&pRtpProxyBuf[iSendNum].rtpTime);
			}
			else
			{
                sendto_sock(sendHandle, pRtpProxyBuf[iSendNum].indesip, pRtpProxyBuf[iSendNum].indesport, (char*)&tPacketBuffer[0], sLen);
                time(&pRtpProxyBuf[iSendNum].rtpTime);
			}
		}
	    readlen += iReadLen;
		if( p.m == 1)
			p.m = 0;
		if( hEvent )
			WaitForSingleObject(hEvent,SleepTime);
		else
			Sleep(SleepTime);
	}
	if( hEvent )
		CloseHandle(hEvent);
	return 1;
}

int RTPPlayG723File(int iCodec, CONNECTION &l_pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved)
{
	int		nLen = 0;
	unsigned char	*pSendBuf=NULL;
	int	rtp_return;
	int iFileLanguagePage = -1;
	int iFileCodecPage = -1;
	int iFileNum = -1;
	nLen = GetMentData(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, pSendBuf);
	if( nLen <= 0 || !pSendBuf)
		return -1;
	
	//rtp_return = rtpWriteFromFile(l_pCon, tSendBuf, nLen, iState,iReserved);
	rtp_return = rtpWriteFromFile(iCodec,l_pCon, pSendBuf, nLen, iState,iReserved);
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


int RtpPlay(int iCodec, 
			CONNECTION &pCon, 
			char * szFileName, 
			char * sLanguage, 
			int & iState,
			int iSkipState, 
			int &iReserved)
{
	return RTPPlayG723File(iCodec, pCon, szFileName, sLanguage, iState, iSkipState, iReserved);
}

void free_rtp_buff()
{
    if( pRtpProxyBuf )
        free(pRtpProxyBuf);
}

int reset_rtp_info(int iCount)
{
    int i = 0;
    rtp_manager_lock();
	if( iCount >= rtp_num )
	{
		i = iCount - rtp_num;
		if( pRtpProxyBuf[i].outdesport > 0 )
		{
			memset(&pRtpProxyBuf[i].outdesip, 0, 24);
			pRtpProxyBuf[i].outdesport = 0;
			pRtpProxyBuf[i].rtpStamp = 0;
			pRtpProxyBuf[i].rtpTime = 0;    
			pRtpProxyBuf[i].outssrc = 0;
			pRtpProxyBuf[i].seq = 0;
			if( pRtpProxyBuf[i].outfp )
				fclose(pRtpProxyBuf[i].outfp);
			pRtpProxyBuf[i].outfp = NULL;
		}
	}
	else
	{
		i = iCount;
		if( pRtpProxyBuf[i].indesport > 0 )
		{
			memset(&pRtpProxyBuf[i].indesip, 0, 24);
			pRtpProxyBuf[i].indesport = 0;
			pRtpProxyBuf[i].rtpStamp = 0;
			pRtpProxyBuf[i].rtpTime = 0;
			pRtpProxyBuf[i].inssrc = 0;
			pRtpProxyBuf[i].seq = 0;
			if( pRtpProxyBuf[i].infp )
				fclose(pRtpProxyBuf[i].infp);
			pRtpProxyBuf[i].infp = NULL;
		}
	}
    rtp_manager_unlock();
    return i;
}

int set_rtp_in(char *ip, int port, int call_num)
{
    int i = call_num;
    time_t tval;
    
    time(&tval);
    rtp_manager_lock();

	if(! pRtpProxyBuf[i].indesport )
    {

		pRtpProxyBuf[i].seq = 0;
		pRtpProxyBuf[i].inssrc = tval * port;
        pRtpProxyBuf[i].indesport	=	port;
        sprintf(pRtpProxyBuf[i].indesip, "%s", ip);
        rtp_manager_unlock();
        return pRtpProxyBuf[i].outRtpPort;
    }
    rtp_manager_unlock();
    return 1;
}

int set_rtp_out(char *ip,int port,int call_num,int iCodec)
{
    int i = 0;
    time_t tval;
    time(&tval);
    
	i = call_num-rtp_num;
    
	rtp_manager_lock();
	
	pRtpProxyBuf[i].outssrc = tval * port;
    pRtpProxyBuf[i].outdesport = port;
    sprintf(pRtpProxyBuf[i].outdesip,"%s", ip);
	if( bEnableRTPChannelRecord )
	{
		char fname[256];
		char fext[16];
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
/*
		if( memcmp( channelName, "g711Ulaw", 8) == 0)
		{
			s->iChannelCodec = 1;
		}
		else if( memcmp( channelName, "g711Alaw", 8) == 0)
		{
			s->iChannelCodec = 2;
		}
		else if( memcmp( channelName, "g7231", 5) == 0)
		{
			s->iChannelCodec = 0;
		}

*/		
		if( iCodec == 2 )
			wsprintf(fext,"%s", "g711a");
		else if( iCodec == 1)
			wsprintf(fext,"%s", "g711u");
		else if( iCodec == 3)
			wsprintf(fext,"%s", "g729");
		else
			wsprintf(fext,"%s", "g7231");
		wsprintf(fname, ".\\voice\\%4d%02d%02d%02d%02d%02d%04d-%03d-%s.%s",
								sysTime.wYear, 
								sysTime.wMonth,
								sysTime.wDay,
								sysTime.wHour,
								sysTime.wMinute,
								sysTime.wSecond,
								sysTime.wMilliseconds,
								call_num, ip, fext);
		pRtpProxyBuf[i].outfp = fopen(fname, "a+b");
		wsprintf(fname, ".\\voice\\%4d%02d%02d%02d%02d%02d%04d-%03d-%s.%s",
								sysTime.wYear, 
								sysTime.wMonth,
								sysTime.wDay,
								sysTime.wHour,
								sysTime.wMinute,
								sysTime.wSecond,
								sysTime.wMilliseconds,
								call_num-rtp_num, ip, fext);
		pRtpProxyBuf[i].infp = fopen(fname, "a+b");
	}	
	
    rtp_manager_unlock();
    return pRtpProxyBuf[i].inRtpPort;
}

static SKYTHREAD rtp_in_manager(SKYINVAL parg)
{
    int i, ret,len;
    fd_set rfd;
    struct timeval tv;
    unsigned char rbuff[512];
    char dip[24];
    int dport;

    tv.tv_sec = 0;
    tv.tv_usec = 50;
    while(rtp_manager_flag)
    {
        FD_ZERO(&rfd);
        for( i = 0; i < rtp_num; i++)
        {
            if( pRtpProxyBuf[i].inRtpHandle )
                FD_SET(pRtpProxyBuf[i].inRtpHandle, &rfd);
        }
        ret = select((pRtpProxyBuf[rtp_num-1].inRtpPort+1), &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
            for( i = 0; (i < rtp_num)&&rtp_manager_flag; i++)
            {
                if(FD_ISSET(pRtpProxyBuf[i].inRtpHandle, &rfd))
                {
					rtp_manager_lock();
					if( pRtpProxyBuf[i].inRtpPort )
					{
						len = recvfrom_sock(pRtpProxyBuf[i].inRtpHandle,
							(char*)&rbuff[0],
							256, (char*)&dip[0], &dport);
						if( pRtpProxyBuf[i].outdesport > 0 && len > 12)
						{
							rtp_hdr_t p;
							uncapRTPHeader((unsigned char*)&rbuff[0], 12, &p);
							p.ssrc = pRtpProxyBuf[i].outssrc;
							capRTPHeader(p, (unsigned char*)&rbuff[0], 12);
							sendto_sock(pRtpProxyBuf[i].outRtpHandle,
								pRtpProxyBuf[i].outdesip,
								pRtpProxyBuf[i].outdesport,
								(char*)&rbuff[0], len);
							time(&pRtpProxyBuf[i].rtpTime);
							if( bEnableRTPChannelRecord )
							{
								int iMax;
								if( pRtpProxyBuf[i].infp ) 
								{
									if( p.pt == G7231 )
										iMax = G723FRAMELENGTH;
									else if ( p.pt == PCMU || p.pt == PCMA)
										iMax = G711FRAMELENGTH;
									else//(p.pt==G729)
										iMax = G729FRAMELENGTH;
									if( iMax == len-12 )
									{
										time_t t;
										time(&t);
										fwrite(&t, sizeof(time_t), 1, pRtpProxyBuf[i].infp);
										fwrite(rbuff+12, 1, len-12, pRtpProxyBuf[i].infp);
									}
								}
							}
						}
					}
					rtp_manager_unlock();
					break;
                }
            }
        }
    }
    sky_thread_close(h_inRTPMANAGER);
    return THREAD_RETURN;
}

static SKYTHREAD rtcp_in_manager(SKYINVAL parg)
{
#if defined USE_INRTCP_REPLY
    int i, ret,len;
    fd_set rfd;
    struct timeval tv;
    unsigned char rbuff[256];
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    FD_ZERO(&rfd);
    while(rtp_manager_flag)
    {
        for( i = 0; i < rtp_num; i++)
        {
            if( pRtpProxyBuf[i].inRtcpHandle )
                FD_SET(pRtpProxyBuf[i].inRtcpHandle, &rfd);
        }
        ret = select(pRtpProxyBuf[rtp_num-1].inRtcpPort+1, &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
            for( i = 0; (i < rtp_num)&&rtp_manager_flag; i++)
            {
                if(FD_ISSET(pRtpProxyBuf[i].inRtcpHandle, &rfd))
                {
                    len = recvfrom_sock(pRtpProxyBuf[i].inRtcpHandle,
                                        (char*)&rbuff[0],
                                        256, NULL, NULL);
                    if( len > 0)
                    {
                        if( pRtpProxyBuf[i].outdesport > 0)
                        {
                            sendto_sock(pRtpProxyBuf[i].outRtcpHandle,
                                pRtpProxyBuf[i].outdesip,
                                pRtpProxyBuf[i].outdesport+1,
                                (char*)&rbuff[0], len);
                        }
                    }
                }
            }
        }
    }
#endif // USE_INRTCP_REPLY
    sky_thread_close(h_inRTCPMANAGER);
    return THREAD_RETURN;
}

static SKYTHREAD rtp_out_manager(SKYINVAL parg)
{
    int i, ret,len;
    fd_set rfd;
    struct timeval tv;
    //unsigned char rbuff[256];
	char rbuff[512];
    //FD_ZERO(&rfd);
#if 1
	::SetThreadPriority(h_outRTPMANAGER, THREAD_PRIORITY_TIME_CRITICAL);
	
    while(rtp_manager_flag)
    {
	    FD_ZERO(&rfd);
        for( i = 0; i < rtp_num; i++)
        {
            if( pRtpProxyBuf[i].outRtpHandle )
                FD_SET(pRtpProxyBuf[i].outRtpHandle, &rfd);
        }
		tv.tv_sec = 0;
	    tv.tv_usec = 15;
        ret = select(pRtpProxyBuf[rtp_num-1].outRtpPort+1, &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
            for( i = 0;(i<rtp_num)&&rtp_manager_flag; i++)
            {
                if(FD_ISSET(pRtpProxyBuf[i].outRtpHandle, &rfd))
                {
					rtp_manager_lock();
					memset(rbuff, 0, 512);
					if( pRtpProxyBuf[i].outRtpPort )
					{
						len = recvfrom_sock(pRtpProxyBuf[i].outRtpHandle,
							(char*)&rbuff[0],
							256, NULL, NULL);
						rtp_hdr_t p;
						if( pRtpProxyBuf[i].indesport > 0 && len > 12)
						{
							uncapRTPHeader((unsigned char*)&rbuff[0], 12, &p);
							p.ssrc = pRtpProxyBuf[i].inssrc;
							capRTPHeader(p, (unsigned char*)&rbuff[0], 12);
							sendto_sock(pRtpProxyBuf[i].inRtpHandle,
									pRtpProxyBuf[i].indesip,
									pRtpProxyBuf[i].indesport,
									(char*)&rbuff[0], len);
							if( bEnableRTPChannelRecord )
							{
								int iMax;
								if( pRtpProxyBuf[i].outfp )
								{
									if( p.pt == G7231 )
										iMax = G723FRAMELENGTH;
									else if(p.pt==G729)
										iMax = G729FRAMELENGTH;
									else//( p.pt == PCMU || p.pt == PCMA)
										iMax = G711FRAMELENGTH;
									if( iMax == len-12 )
									{
										time_t t;
										time(&t);
										fwrite(&t, sizeof(time_t), 1, pRtpProxyBuf[i].outfp);
										fwrite(rbuff+12, 1, len-12, pRtpProxyBuf[i].outfp);
									}
								}
							}
						}
					}
					rtp_manager_unlock();
					break;
                }
            }
        }
    }
#endif
    sky_thread_close(h_outRTPMANAGER);
    return THREAD_RETURN;
}
static SKYTHREAD rtcp_out_manager(SKYINVAL parg)
{
#if defined USE_OUTRTCP_REPLY
    int i, ret,len;
    fd_set rfd;
    struct timeval tv;
    unsigned char rbuff[256];
    tv.tv_sec = 0;
    tv.tv_usec = 20;
    FD_ZERO(&rfd);
    while(rtp_manager_flag)
    {
        for( i = 0;(i<rtp_num)&&rtp_manager_flag; i++)
        {
            if( pRtpProxyBuf[i].outRtcpHandle )
                FD_SET(pRtpProxyBuf[i].outRtcpHandle, &rfd);
        }

        ret = select(pRtpProxyBuf[rtp_num-1].outRtcpPort+1, &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
            for( i = 0;(i<rtp_num)&&rtp_manager_flag; i++)
            {
                if(FD_ISSET(pRtpProxyBuf[i].outRtcpHandle, &rfd))
                {
                    len = recvfrom_sock(pRtpProxyBuf[i].outRtcpHandle,
                                        (char*)&rbuff[0],
                                        256, NULL, NULL);
                    if( len > 0)
                    {
                        if( pRtpProxyBuf[i].indesport > 0)
                        {
                            sendto_sock(pRtpProxyBuf[i].inRtcpHandle,
                                pRtpProxyBuf[i].indesip,
                                pRtpProxyBuf[i].indesport+1,
                                (char*)&rbuff[0], len);
                        }
                    }
                }
            }
        }
    }
#endif // USE_OUTRTCP_REPLY
    sky_thread_close(h_outRTCPMANAGER);
    return THREAD_RETURN;
}


int start_rtp_manager(int u_num, int start_port)
{
    int i;
    rtp_num = u_num;
	RTPMAX_USER = u_num;
    int memsize = sizeof(rtp_proxy_buf_t) *u_num;
    if( start_port > 0)
        rtp_start_port = start_port;
    else
        rtp_start_port = RTP_OUT_DEF_PORT_START;
    pRtpProxyBuf = NULL;
    pRtpProxyBuf = (p_rtp_proxy_buf_t)malloc(memsize);
    if( !pRtpProxyBuf )
        return 0;
    enable_rtp_manager_flag();
    for(i= 0;i<u_num;i++)
    {
        reset_rtp_buf_basic(&pRtpProxyBuf[i]);
        pRtpProxyBuf[i].inRtpPort = rtp_start_port + i*2;
        pRtpProxyBuf[i].inRtcpPort = rtp_start_port + i*2 + 1;
        pRtpProxyBuf[i].outRtpPort = rtp_start_port + /*RTP_RTCP_GAP + */ rtp_num*2+i*2;
        pRtpProxyBuf[i].outRtcpPort = rtp_start_port + /*RTP_RTCP_GAP + */rtp_num*2+i*2 + 1;
        pRtpProxyBuf[i].outRtpHandle = create_udp_sock(pRtpProxyBuf[i].outRtpPort);
        pRtpProxyBuf[i].outRtcpHandle = create_udp_sock(pRtpProxyBuf[i].outRtcpPort);
		pRtpProxyBuf[i].inRtpHandle = create_udp_sock(pRtpProxyBuf[i].inRtpPort);
        pRtpProxyBuf[i].inRtcpHandle = create_udp_sock(pRtpProxyBuf[i].inRtcpPort);
    }
    h_outRTPMANAGER = sky_thread_create(rtp_out_manager,NULL, 0, 0);
    if( h_outRTPMANAGER )
    {
        h_inRTCPMANAGER = sky_thread_create(rtcp_in_manager,NULL, 0, memsize +1000);
        if(h_inRTCPMANAGER)
        {
			h_inRTPMANAGER = sky_thread_create(rtp_in_manager,NULL, 0, 0);
            if( h_inRTPMANAGER )
            {
                h_outRTCPMANAGER = sky_thread_create(rtcp_out_manager,NULL, 0, memsize +1000);
                if( h_outRTCPMANAGER )
                    return 1;
            }
        }
    }
    disable_rtp_manager_flag();
	HANDLE hEvent=CreateEvent(NULL, FALSE, FALSE, "START");;
	if( hEvent )
	{
		WaitForSingleObject(hEvent, 20);
		CloseHandle(hEvent);
	}else
	    Sleep(200);
    free(pRtpProxyBuf);
    return 0;
}