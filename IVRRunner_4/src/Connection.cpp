#include "Connection.h"

///////////////////////////////////////////////////////////////////////////////////////
// start of file
// file name	: Connection.cpp
// date			: 2006/06/12 19:31 swhors@naver.com

_channel::_channel()
{
	init();
}

_channel::~_channel()
{
	init();
}

void _channel::init()
{
	this->haCall=NULL;
	this->hsChan=NULL;
	this->sNum = 0;
	this->port = 0;
	this->state = 0;
	this->stateMode = 0;
	this->rate = 0;
	this->handle = 0;
	this->dpt = 0;
	this->uIP = 0;
	this->uPort = 0;
	memset(this->channelName, 0, 10);
}

tag_CONNECTION::tag_CONNECTION()
{
	init();
}

tag_CONNECTION::~tag_CONNECTION()
{
	init();
}

void tag_CONNECTION::init()
{
	this->bCallReusing = 0;
	this->bInChannelConnect = 0;
	this->bRetrialCall = 0;
	this->bOpendChannel = 0;
	this->bTimerSet = 0;
	this->bUsedThisCall = 0;
	this->bVoiceRec = 0;
	memset(src_e164, 0, 36);
	memset(des_e164, 0, 36);
	memset(cID, 0, 18);
	memset(&GlobalFSDataRequested, 0, sizeof(cmFastStartMessage));
	this->iCallMaster = -1;
	this->iCallState = 0;
	//this->hCallStartEvent = NULL;
	this->thread_sce = NULL;
	memset(this->in, 0, sizeof(channel) *10);
	memset(this->out, 0, sizeof(channel) *10 );
	this->hsCall = NULL;
	memset( &this->overlapState,0, sizeof(overlapStructT));
	//CRITICAL_SECTION	cs_Call_PROC;
	//CRITICAL_SECTION	cs_H245_CTRL;
	this->iChannelCodec = 0;
	this->defOutCallDuration = 0;
	this->m_iFastStart = 0;
	this->m_iH245Tunneling = 1;
	this->m_iInputDirection = 0; //default to user.
}
/*
	BOOL	bInChannelConnect; // inbound 채널을 유지할 것인지를 가짐.
								// 기본값 : 0 - 유지하지 않음.
								// 1 : outbound 콜의 해제시 같이 해제 되지 않음.
*/

///////////////////////////////////////////////////////////////////////////////////////
// end of file...
//
