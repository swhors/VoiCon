#include	"stdafx.h"
#include	"mediathread.h"
#include	"usermsg.h"
#include    "process.h"

#include	<math.h>
#include	"rtp.h"
#include	"rtcp.h"
#include	"payload.h"
#include	"acmcodec.h"
#include    "G729Codec.h"
#include    "phonevoice.h"
#include    "G711Codec.h"
#include	"G7231Codec.h"

//#include    "dtmf_def.h"

#include    "AudioInterface.h"

#ifndef UINT16
typedef unsigned int UINT16;
#endif

unsigned long ghMediaThr = 0;
unsigned short gRTPseq = 0;
DWORD gMediaThrID = 0;
HWND gMediaWnd=0;
HWND gMainDlgHwnd;
DWORD theTime;

CAudioInterface *gUserWaveIn=NULL, *gUserWaveOut=NULL;

CG729Codec *gDecode729Codec = NULL, *gEncode729Codec = NULL;
//CACMCodec *gDecodeCodec=NULL, *gEncodeCodec=NULL;
CG711Codec *gDecodec711Codec = NULL, *gEncode711Codec = NULL;
CG7231Codec *gDecodeG7231Codec = NULL, *gEncodeG7231Codec = NULL;

BOOL bplay1;

BOOL bDevForceEnd = 0;

static int playbuf1, playbuf2, playbuf3;
#define MAX_BUF_NUM		3000
typedef struct _voice_codec_buf
{
	BYTE buf[320];
    BYTE orData[20];
    int  iOpt;
	int  chksum;
}VOICECODECBUF,*PVOICECODECBUF;

VOICECODECBUF g729Voice[MAX_BUF_NUM];

extern CRITICAL_SECTION PalyCritical,PalyCritical1,PalyCritical2;

unsigned int gUsedCodec;

#if 0
unsigned short gDTMFbuf[4][240];
int gDTMFCnt=0;
void MakeDTMFImg16(char dtmfc);
#endif

int waitRtpPort;

HRTPSESSION gOutRTP=NULL, gInRTP=NULL;
HRTCPSESSION gOutRTCP=NULL, gInRTCP=NULL;

void RTPEventHandler(HRTPSESSION hRTP, void* context);

#define DEC_TIME_CHK
#define COUNT_LOG

UINT MediaThread(LPVOID arg);

void send_media(void *rData, int len, int type, int opt);

unsigned long StartMediaThread(HWND  hDlgHwnd)
{
	gMainDlgHwnd = hDlgHwnd;
	AfxBeginThread(MediaThread, NULL, THREAD_PRIORITY_HIGHEST);
	return ghMediaThr;
}

UINT MediaThread(LPVOID arg)
{
	MSG msg;
    WNDCLASS wc;
//	gEncodeCodec = gDecodeCodec = NULL;

	gUserWaveIn = gUserWaveOut = NULL;

//	waitRtpPort = 0;
	gUsedCodec = 0;

	gMediaThrID = GetCurrentThreadId();
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc    = MediaWinProc;
    wc.hInstance      = ghInstance;
    wc.lpszClassName  = "MediaWin";
    RegisterClass(&wc);
    bDevForceEnd = 0;
    gDecode729Codec = new CG729Codec;
    gDecode729Codec->Open(0);
    gEncode729Codec = new CG729Codec;
    gEncode729Codec->Open(1);
	gDecodec711Codec = new CG711Codec;
	gEncode711Codec = new CG711Codec;
	gDecodeG7231Codec = new CG7231Codec;
	gDecodeG7231Codec->Open(0);
	gEncodeG7231Codec = new CG7231Codec;
	gEncodeG7231Codec->Open();
	
	bplay1 = FALSE;
    
	playbuf2 = 0;
	playbuf1 = 0;
	playbuf3 = 0;

	rtcpInit();
	rtpInit();
    
 	gUserWaveIn = new CAudioInterface;
	gUserWaveOut = new CAudioInterface;
	
	gMediaWnd = CreateWindow ("MediaWin", NULL,WS_OVERLAPPED | WS_MINIMIZE,
			  0, 0, 0, 0, NULL, NULL, ghInstance, NULL);
	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return NULL;
}


LRESULT WINAPI MediaWinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int ret;
	switch(uMsg)
    {
		case UM_REQINRTP:
			if(!gInRTP)
			{
				//waitRtpPort = (int)lParam;
				gInRTP = rtpOpen(waitRtpPort, 1, 0xff);
				gInRTCP = rtcpOpen(waitRtpPort+1, 0, "testrtp");
				rtpSetEventHandler(gInRTP, RTPEventHandler, NULL);
			}
			break;
		case UM_REQOUTRTP:
			/*
			아웃 RTP채널을 연 후에, 음성 입력을 시작합니다.
			*/
			if(!gOutRTP)
			{
				// 아웃 RTP 채널을 엽니다.
#if 0
				int port = (int)lParam;
				gOutRTP = rtpOpen(port, 1, 0xff);
    			gOutRTCP = rtcpOpen(rtpGetSSRC(gOutRTP), port+1, "rmttest");
#endif
				gRTPseq = 0;

				theTime=GetTickCount();
				gRTPseq = 1;
				if( gUserWaveIn )
                    gUserWaveIn->recordStart();
			}
			break;
		case UM_CLOSEINRTP:
			{
				if(gInRTP)
				{
					ret = rtpClose(gInRTP);
				}
				if(gInRTCP)
				{
					ret = rtcpClose(gInRTCP);
				}
				for( int i = 0; i < MAX_BUF_NUM; i++)
				{
					g729Voice[i].iOpt = 0;
				}
				playbuf1 = 0;
				playbuf2 = 0;
				playbuf3 = 0;
				gInRTP = 0;
				gInRTCP = 0;
			}
			break;
		case UM_CLOSEOUTRTP:
			//
			//마이크 음성 입력을 정지 합니다.
			//
//
			//if( gOutRTP )
			//	ret = rtpClose(gOutRTP);
			//if( gOutRTCP )
			//	ret = rtcpClose(gOutRTCP);
			//gOutRTP = 0;
			//gOutRTCP = 0;
//
			break;
		case UM_SETINRTCP_RMTADDR:
			{
				// 나의 RTCP 채널 정보를 입력 대기 RTCP 채널 정보에 넣습니다.
			    if(gInRTCP)
					rtcpSetRemoteAddress(gInRTCP, (UINT32)wParam, (UINT16)lParam);
			    break;
			}
		case UM_SETINRTP_RMTADDR:
			{
				// 나의 RTP 채널 정보를 입력 대기 RTP 채널 정보에 넣습니다.
				if(gInRTP)
					rtpSetRemoteAddress(gInRTP, (UINT32)wParam, (UINT16)lParam);
			    break;
			}
		case UM_SETOUTRTP_RMTADDR:
			{
				// 상대방의 RTP 채널 정보를 아웃 RTP 채널 정보에 넣습니다.
				//
			    //if(gOutRTP) 
				//    rtpSetRemoteAddress(gOutRTP, (UINT32)wParam, (UINT16)lParam);
				if(gInRTP)
					rtpSetRemoteAddress(gInRTP, (UINT32)wParam, (UINT16)lParam);
			    break;
			}
		case UM_SETOUTRTCP_RMTADDR:
			{
				// 상대방의 RTCP 채널 정보를 아웃 RTCP 채널 정보에 넣습니다.
			    //if(gOutRTCP)
				//	rtcpSetRemoteAddress(gOutRTCP, (UINT32)wParam, (UINT16)lParam);
			    if(gInRTCP)
					rtcpSetRemoteAddress(gInRTCP, (UINT32)wParam, (UINT16)lParam);
			    break;
			}
		case UM_OPENINDEV:
			{
				gUsedCodec = (unsigned int)wParam;
				if( gUsedCodec == 3 )
				    gEncode711Codec->ulaw_enable();
				else if( gUsedCodec == 4 )
				    gEncode711Codec->ulaw_disable();
				if( gUserWaveIn )
				{
					gUserWaveIn->InDevOpen(gUsedCodec);
				}
				gUserWaveIn->codecType = gUsedCodec;
			}
			break;
		case UM_OPENOUTDEV:
			{
				gUsedCodec = (unsigned int)wParam;
                bplay1 = TRUE;
				if( gUsedCodec == 2)//case 2: // g729.a
                    playbuf3 = 0;
				else if( gUsedCodec == 3 )
				    gEncode711Codec->ulaw_enable();
    			else if( gUsedCodec == 4 )
				    gEncode711Codec->ulaw_disable();
				if( gUserWaveOut )
				    gUserWaveOut->OutDevOpen(gUsedCodec);
				gUserWaveOut->codecType = gUsedCodec;
			}
			break;
		case UM_CLOSEOUTDEV:
			bplay1 = FALSE;
			Sleep(10);
			gUsedCodec = 0;
			if( gUserWaveOut )
				gUserWaveOut->close();
			break;
		case UM_CLOSEINDEV:
			if( gUserWaveIn )
			{
			    gUserWaveIn->recordStop();
				gUserWaveIn->close();
			}
			gUsedCodec = 0;
            break;
		case UM_SENDDTMF:
//			gDTMFCnt = 4;
//			MakeDTMFImg16((char)wParam);
			break;
		case UM_PLAYDTMF:
			{
//				char dtmf = (char)wParam;
//				gUserWaveOut->SendDTMFinBand(dtmf);
			}
			break;
		case UM_SENDMEDIA:
			send_media((unsigned char*)wParam, lParam, gUsedCodec, 0);
			break;
		case UM_MEDIAQUIT:
			TRACE("meadia win proc wm_quit\n");
			rtpEnd();
			rtcpEnd();
			//if(gEncodeCodec)
			//{
			//	gEncodeCodec->Close();
			//	delete gEncodeCodec;
			//	gEncodeCodec = NULL;
			//}
			//if(gDecodeCodec)
			//{
			//	gDecodeCodec->Close();
			//	delete gDecodeCodec;
			//	gDecodeCodec = NULL;
			//}
			if( gDecodeG7231Codec )
			{
				gDecodeG7231Codec->Close();
				delete gDecodeG7231Codec;
				gDecodeG7231Codec = NULL;
			}
			if( gEncodeG7231Codec )
			{
				gEncodeG7231Codec->Close();
				delete gEncodeG7231Codec;
				gEncodeG7231Codec = NULL;
			}
			
			if( gDecode729Codec)
			{
				delete gDecode729Codec;
				gDecode729Codec = NULL;
			}
			if( gEncode729Codec )
			{
				delete gEncode729Codec;
				gEncode729Codec = NULL;
			}
			if( gDecodec711Codec )
			{
				delete gDecodec711Codec;
				gDecodec711Codec = NULL;
			}
			if( gEncode711Codec )
			{
				delete gEncode711Codec;
				gEncode711Codec = NULL;
			}
			if( gUserWaveIn )
			{
				//gUserWaveIn->close();
				delete gUserWaveIn;
				gUserWaveIn = NULL;
			}
			if( gUserWaveOut )
			{
				delete gUserWaveOut;
				gUserWaveIn = NULL;
			}
			DestroyWindow(gMediaWnd);
			UnregisterClass("MediaWin", ghInstance);
			gMediaWnd =  NULL;
			break;
		default:
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }
    return 0l;
}

void send_media(void *rData, int len, int type, int opt)
{
	int ret;
    //tRTPInfo *rtpinfo;
	rtpParam rtpparam;
	unsigned char buf[400];

	if(gInRTP)
	{
		rtpparam.sequenceNumber = gRTPseq++;
		//rtpparam.timestamp = (GetTickCount()-theTime)<<3;
		if( !opt )
		    rtpparam.marker = 1;
		else
			rtpparam.marker = 0;
		rtpparam.payload = PCMU;
		if( type == 1)
		{
			rtpparam.payload = G7231;
			theTime = theTime + 240;
		    rtpparam.timestamp = theTime;
		}
		else if( type == 2)
		{
			rtpparam.payload = G729;
			theTime = theTime + 160;
		    rtpparam.timestamp = theTime;
		}
		else if( type == 4 || type == 3)
		{
			if( type == 4 )
			    rtpparam.payload = PCMA;
			theTime = theTime + 160;
		    rtpparam.timestamp = theTime;
		}
#if 0
		else if( type == RTP_DTMF_2833_TYPE )
		{
		    rtpparam.payload = RTP_DTMF_2833_TYPE;
			//theTime = theTime + 160;
		    rtpparam.timestamp = theTime;
		}
#endif
		rtpparam.len = 12 + (int)len;
		rtpparam.sByte = 12;
		memset(buf, 0, 400);
		memcpy(buf+12, (void*)rData, len);
		ret = rtpWrite(gInRTP, buf, rtpparam.len, &rtpparam);
	}
}

void RTPEventHandler(HRTPSESSION hRTP, void* context)
{
	unsigned char buf[512];	
    int ret;
    rtpParam param;
	
	memset(buf, 0, 512);
    ret = rtpRead(hRTP, buf, sizeof(buf), &param);
	if( param.sByte <= 0 )
		return;
	gUserWaveOut->OnDataPlay((PBYTE)&buf[param.sByte], param.len-param.sByte);
}

#if 0
int get_rtp_handle(int dir)
{
	if(gInRTP)
	{
		rtpSession *rtpS=(rtpSession*)gInRTP;
        return rtpS->socket;
	}
    return 0;
}
#endif

#if 0
void MakeDTMFImg16(char dtmfc)
{
	unsigned f1, f2;
	int i,c,j;
	GetDTMFFrq(dtmfc, &f1, &f2);
	
	for(j=0;j<240;j++)
		gDTMFbuf[0][j] = 0;
	for(c=0, i=1;i<4;i++)
	{
		for(j=0;j<240;j++)
		{
			gDTMFbuf[i][j] = (unsigned short)(16388*sin(c*2*3.14*f1/8000) + 16388*sin(c*2*3.14*f2/8000));
			c++;
		}
	}
}
#endif

#if 0
void GetDTMFFrq(char dtmf, unsigned int *f1, unsigned int *f2)
{
	switch(dtmf)
	{
		case '1':
		case '2':
		case '3':
		case 'A':
			*f1=697;
			break;
		case '4':
		case '5':
		case '6':
		case 'B':
			*f1=770;
			break;
		case '7':
		case '8':
		case '9':
		case 'C':
			*f1=852;
			break;
		case '*':
		case '0':
		case '#':
		case 'D':
			*f1=941;
			break;
	}

	switch(dtmf)
	{
		case '1':
		case '4':
		case '7':
		case '*':
			*f2=1209;
			break;
		case '2':
		case '5':
		case '8':
		case '0':
			*f2=1336;
			break;
		case '3':
		case '6':
		case '9':
		case '#':
			*f2=1477;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
			*f2=1633;
			break;
	}

}
#endif