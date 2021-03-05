#ifndef __MEDIATHREADH__
#define __MEDIATHREADH__

#include "acmcodec.h"
#include "rtp.h"
#include "rtcp.h"

typedef struct
{
	unsigned short rtpPort;
	unsigned short rtcpPort;
} tRTPInfo;

typedef struct
{
	rtpParam rm;
	unsigned char buf[24];
} tRTPPkt;

extern unsigned long ghMediaThr;
extern DWORD gMediaThrID;
extern HWND gMediaWnd;
extern CACMCodec *gEncodeCodec;
extern CACMCodec *gDencodeCodec;
void GetDTMFFrq(char dtmf, unsigned int *f1, unsigned int *f2);
extern unsigned short gDTMFbuf[4][240];
extern int gDTMFCnt;
void MakeDTMFImg16(char dtmfc);

LRESULT WINAPI MediaWinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
//void __cdecl MediaThread(void *arg);
unsigned long StartMediaThread(HWND hWnd);

#endif
