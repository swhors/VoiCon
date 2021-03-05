// AcmG711u.cpp: implementation of the CAcmG711u class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "audio.h"
#include "AcmG711u.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcmG711u::CAcmG711u(char *fName)
{
	this->EnableInWave();
	this->EnableOutG711U();
	this->SetSRFileName(fName);
	//CACMWAVE::CACMWAVE(fName);
}

CAcmG711u::~CAcmG711u()
{
	CACMWAVE::~CACMWAVE();
}

int CAcmG711u::open(IN char *fileName,IN int bRead)
{
	return CACMWAVE::open(fileName, bRead);
}
