// AcmG711a.cpp: implementation of the CAcmG711a class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "audio.h"
#include "AcmG711a.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcmG711a::CAcmG711a(char *fName)
{
	this->EnableInWave();
	this->EnableOutG711A();
	this->SetSRFileName(fName);
	//CACMWAVE::CACMWAVE(fName);
}

CAcmG711a::~CAcmG711a()
{
	CACMWAVE::~CACMWAVE();
}

int CAcmG711a::open(IN char *fileName,IN int bRead)
{
	return CACMWAVE::open(fileName,bRead);
}
