#ifndef RTPTEST_H
#define RTPTEST_H

#define UINT16 unsigned short
#define UINT32	unsigned int

void lRtpInit(HWND hWnd, int max_call,UINT32 uIP);
void lRtpEnd(void);

int lRtpOpen(int session,char* name,int startPort);
int lRtpClose(int session);

void lRtpSetRtp(int session, UINT32 ip,UINT16 port);
void lRtpSetRtcp(int session, UINT32 ip,UINT16 port);

void lRtpTimer(int param);
void lRtpWriteRTP(BOOL write);
UINT32 lRtpRate(int session);
void PlayMoney(int iSession, int money);
void PlayTime(int iSession, int time, int iLanguage);
HRTPSESSION getRTPSession( int iNum);
HRTCPSESSION getRTCPSession( int iNum);

#endif