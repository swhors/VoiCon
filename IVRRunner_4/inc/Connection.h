// Connection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "cm.h"

// H450
#ifdef H450
#include "h450.h"
#include "sse.h"
#endif

typedef struct _channel
{
	_channel();
	~_channel();
	void init();
	HAPPCALL    haCall;
	HCHAN       hsChan;
	int         sNum;
	int         port;
	int         state;
	int         stateMode;
	int         rate;
	int         handle;
	UINT		uIP;
	int			uPort;
	BYTE        dpt;
	char        channelName[10];
} channel;

typedef enum
{
	overlapNotInit=0,
		overlapRunning,
		overlapWait,
		overlapIncomplete,
		overlapInProcess
}overlapStateT;

typedef struct 
{
	overlapStateT running;
	int sendingComplete;
	char address[30];
	char baseAddress[50];
}overlapStructT;

typedef struct tag_CONNECTION
{
	tag_CONNECTION();
	~tag_CONNECTION();
	void		init();
	BOOL			bOutIsBusy;		// 아웃 바운드에서 호를 시도한 경우, 해당 아웃바운드 데스티니네이션이 비지 인지를 상태를 갖는 값.
	BOOL			bCallReusing;	//  콜을 다시 사용할 수 있는지의 상태 정보를 가짐.
	int				iCallNumber;	// this call identify number
	BOOL			bUsedThisCall;	// State which this call is used or not used.
	BOOL			bOpendChannel;
	int				iCallMaster;	// default -1, not defined.
	int				iCallState;		// 0 wait
									// 2 connected
									// 1 calling
									// 3 busying
									// 4 outbound call connect
									// 5 disconnect
									// 6 outbound call disconnect
									// 6< warncode;
	char			src_e164[36];
	char			des_e164[36];
	unsigned char	cID[18];
//#ifdef FAST_START
	cmFastStartMessage	GlobalFSDataRequested;
//#endif
	//HANDLE			hCallStartEvent;
	HANDLE			thread_sce;
	channel			in[10];		// in channel information
	channel			out[10];	// out channel information
	HCALL			hsCall;
	BOOL			bTimerSet;
	overlapStructT	overlapState;
	//CRITICAL_SECTION	cs_Call_PROC;
	//CRITICAL_SECTION	cs_H245_CTRL;
	BOOL			bVoiceRec; // if true, will be recorded voice in g7231.
	int				iChannelCodec;
	BOOL			bInChannelConnect; // inbound 채널을 유지할 것인지를 가짐.
										// 기본값 : 0 - 유지하지 않음.
										// 1 : outbound 콜의 해제시 같이 해제 되지 않음.
	BOOL			bRetrialCall; // 이 콜이 Retrial을 사용하는 콜인지에 대한 해당 상태 코드.
	int				defOutCallDuration;
	int				m_iH245Tunneling;
	int				m_iFastStart;
	int				m_iInputDirection; // 0=user, 1=receive, 2=both
}CONNECTION,*PCONNECTION;


typedef struct tag_RTPCHECKBUFF
{
	int nTimer;
	int nFlag;       // 0-유휴상태, 1-RTP전송상태
}RTPCHECKBUFF;

/*  
  ** Customer Query Item
- Cust_no[15]
= Cust_name[40]
- Cust_pw[10]
- cust_pwyn[1]
- cust_stop[1]

  ** CDR Insert Item
- e164[30] 
- call_time - connect start time[14]
- h323[64]
- cust_no[15]
- call_type - default 0[1]
- Dest_no[30]
- Duration - int
- In-IPADDR[20]
*/

typedef struct tag_CALLINFOMATION
{
	char	Cust_No[15];
	char	Cust_Pw[10];
	char	Cust_Pwyn;
	char	Cust_Stop;
	char	E164[30];
	char	Call_Time[14];
	char	H323[64];
	char	Call_Type;
	char	Dest_No[30];
	char    Duration[10]; // type of int
	char	In_Ipaddr[20];
	char	User_GateWay323ID[64];
	char	GateWay_E164[30];
	char	GateWay_H323[64];
	char	GateWay_ID[10];
	char	Temp[128];
}CALLINFOMATION;

#endif // _CONNECTION_H
