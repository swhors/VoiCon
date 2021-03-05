// CallConf.cpp: implementation of the CCallConf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ubidialer.h"
#include "H323_INC\CallConf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallConf::CCallConf()
{

}

CCallConf::~CCallConf()
{

}

void CCallConf::init()
{
	this->m_EnableRecordRtp = FALSE;
	this->m_g729Support = 0;
	this->m_iFastStart = 1;
	this->m_iH245Tunneling = 0;
	this->m_InitOfRtpPort = 2000;
	this->m_EnableG711u = 0;
	this->m_EnableG711a = 0;
	this->m_EnableG7231 = 0;
	this->m_EnableDtmf = 0;
	this->m_EnbaleSilenceSupression = 0;
	this->m_iG7231AudioFrame = 1;
	this->m_iEnableCodecCount = 0;
	memset(this->m_sSelfE164, 0, 64);
}
