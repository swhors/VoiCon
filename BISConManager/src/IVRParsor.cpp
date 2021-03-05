// IVRPasor.cpp : implementation file
//

#include "stdafx.h"
#include "BISConManager.h"
#include "IVRParsor.h"
#include "StateView.h"
#include "MainFrm.h"

//#include "global.h"

CommandData		g_tCommandDataRecv[] =
{
	"Max Allowed Call", 1,
    "default gk ip", 2 ,
	"default gk port", 3, 
	"h323id", 4, 
	"e164id", 5, 
	"enableDebugConsole", 6,
	"enableCallCoupling", 7 ,
	"maxDelayJitter", 8 ,
	"SCENARIO", 9 ,
	"LANGUAGE", 10 ,
	"SEND END", 11, 
	"ttl", 12, 
	"DTMF Input Delay", 13,
	"User Input Wait", 14,
	"OK",		100
};

#define RECV_COMMAND_COUNT	15

CommandData		g_tCommandDataSend[] =
{
	"ivr start", 1,
    "ivr stop", 2 ,
	"ivr isrun", 3, 
	"ivr adduser", 4, 
	"ivr passwd", 5, 
	"view configure", 6,
	"view scenario", 7 ,
	"view call", 8 ,
	"log calllog ture", 9 ,
	"log calllog false", 10 ,
	"log detaillog true", 11 ,
	"log detaillog false", 12 ,
	"reset_call", 13
};

#define SEND_COMMAND_COUNT	13

extern CBISConManagerApp theApp;

CCallStateBuf * pStateBuf;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIVRPasor

CCallStateBuf::CCallStateBuf()
{
	init();
}

CCallStateBuf::~CCallStateBuf()
{
	free();
}

void CCallStateBuf::init()
{
	this->iCallNum = -1;
	this->iStep = 0;
	this->state = 0;
	dE164="none";
	sE164="none";
}

void CCallStateBuf::free()
{
	init();
}

CCallStateBuf &CCallStateBuf::operator=(const CCallStateBuf &c)
{
	this->dE164 = c.dE164;
	this->iCallNum = c.iCallNum;
	this->iStep = c.iStep;
	this->sE164 = c.sE164;
	this->state = c.state;
	return *this;
}

int CCallStateBuf::operator==(const CCallStateBuf &c)
{
	if( this->iCallNum == c.iCallNum )
		return 1;
	return 0;
}

int	CCallStateBuf::operator<(const CCallStateBuf &c)
{
	if( this->iCallNum<c.iCallNum)
		return 1;
	return 0;
}

int	CCallStateBuf::operator>(const CCallStateBuf &c)
{
	if( this->iCallNum>c.iCallNum)
		return 1;
	return 0;
}

void CCallStateBuf::build(int iCallNum,CString src,CString dst,int step,int st)
{
	this->iCallNum = iCallNum;
	this->dE164 = dst;
	this->sE164 = src;
	this->iStep = step;
	this->state = st;
}

CIVRParsor::CIVRParsor()
{
	memset( m_pSendBuff, NULL, 1580 );
	memset( m_pRecvBuff, NULL, 1580 );
	m_nSendLength	=	0;
	m_nSockStatus	=	0;
	m_nComSendCount	=	0;
}

CIVRParsor::~CIVRParsor()
{
}

BEGIN_MESSAGE_MAP(CIVRParsor, CWnd)
	//{{AFX_MSG_MAP(CIVRParsor)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIVRPasor message handlers
void CIVRParsor::ParsePacket(char *packet )
{
	CString str;
//	str.Format("len:%d-%s", strlen(packet), packet );
	str.Format("%s", packet );
	
	//if( theApp.m_pLogView != NULL )
	//	theApp.m_pLogView->AddLogString( str );

	char buff[1580];
	memset(buff, NULL, 1580 );

	int packetlen = strlen(packet);

	if( packetlen == 1 )
	{
		if( packet[0] == m_pSendBuff[m_nSendLength] )
		if( (m_nSendLength + 1 ) == strlen( m_pSendBuff ) )
		{
			sprintf(buff,"\r\n");
			//SendCommand( buff, 2 );
			m_nSendLength = 0;
			memset( m_pSendBuff, NULL, sizeof( m_pSendBuff ));
		}else 
		{
			m_nSendLength++;
			memcpy(buff, m_pSendBuff + m_nSendLength, 1 );
			//SendCommand(buff, 1 );
		}
	
	}else if( packetlen == 2 )
	{

	}
}

COMMANDSTRUCT CIVRParsor::LineAnaly( char *buff )
{

	int		totallength = strlen( buff );
	COMMANDSTRUCT ps;
	ps.linecount	=	0;

	for( int count = 0 ; count < totallength  ; count++)
	{
		if( buff[count] == '\n' )
		{
			ps.linediv[ps.linecount] = count;
			ps.linecount++;

		}
	}

	return ps;
}

void CIVRParsor::SetCallStatus( char *buff )
{
// call state
// 0 wait, 1 calling,2 connected,3 busying, 4 outbound call connect, 5 disconnect
// 6 outbound call disconnect
// "200/%d/%s/%d/%s/%d/", iCallNumber, src_e164, iCallState, des_e164(NONE), Scenario_Step
	char seps[] = "/";
	char *token;
	
	CString src_e164;
	CString dst_e164;
		
	int channel = 0;
	int call_state = 0;
	
	int count = 0;
	int	scenario_step = 0;
	
	token = strtok( buff, seps );

	//	"200/c%d/s%s/t%d/d%s/u%d/",
	while( token)
	{
		switch( *token )
		{
		case 'c':
			channel = atoi( token+1 );
			break;
		case 's':
			src_e164.Format("%s", token+1 );
			break;
		case 't':
			call_state = atoi( token +1 );
			break;
		case 'd':
			dst_e164.Format("%s", token +1);
			break;
		case 'u':
			scenario_step = atoi(token+1);
			break;
		}
		token = strtok( NULL, seps );
	}
	pStateBuf[channel].build(channel, src_e164, dst_e164, scenario_step, call_state);
	if( theApp.m_pStateView != NULL )
		theApp.m_pStateView->StateChange( channel, call_state, src_e164, dst_e164,scenario_step);
}