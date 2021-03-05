#include "stdafx.h"
#include "H323CallInfo.h"

_channel::_channel()
{
	init();
}
_channel::~_channel()
{
}
void _channel::init()
{
	memset(this->channelName, 0, 10);
	this->sNum = 0;
	this->port = 0;
	this->state = 0;
	this->stateMode = 0;
	this->rate = 6400;
	this->haCall = NULL;
	this->handle = 0;
	this->hsChan = NULL;
	this->dpt = 0;
}

CH323CallInfo::CH323CallInfo()
{
}

CH323CallInfo::~CH323CallInfo()
{
}


void CH323CallInfo::init()
{
	bCalled = FALSE;
	iRtcpPort = 0;
	iRtpPort = 0;
	memset( m_sOtherE164 , 0, 64);
	memset( m_sSelfE164 , 0, 64);
	this->iChannelCodec = -1;
	this->bOpendChannel = 0;
	this->bCalled = 0;
	this->bCalling = 0;
	for(int i = 0; i < 10 ; i++)
	{
		this->in[i].init();
		this->out[i].init();
	}
	this->hsCall = NULL;
	this->m_iFastStart = 0;
	this->m_iH245Tunneling = 0;
	//bUsedThisCall = FALSE;
	bCallReusing = FALSE;
	this->bOpendChannel = FALSE;
}

void CH323CallInfo::release()
{
	init();
}
