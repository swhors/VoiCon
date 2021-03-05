// AcmG7231.cpp: implementation of the CAcmG7231 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "audio.h"
#include "AcmG7231.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcmG7231::CAcmG7231(char *fName)
{
	this->EnableInWave();
	this->EnableOutG7231();
	this->SetSRFileName(fName);
	//CACMWAVE::CACMWAVE(fName);
}

CAcmG7231::~CAcmG7231()
{
	CACMWAVE::~CACMWAVE();
}

int CAcmG7231::open(IN char *fileName,IN int bRead)
{
	return CACMWAVE::open(fileName,bRead);
}
