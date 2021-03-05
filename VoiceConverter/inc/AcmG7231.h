// AcmG7231.h: interface for the CAcmG7231 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACMG7231_H__5CE932B8_CA5B_4A34_AAC3_B99C81BA092A__INCLUDED_)
#define AFX_ACMG7231_H__5CE932B8_CA5B_4A34_AAC3_B99C81BA092A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ACMWAVE.h"

class CAcmG7231 : public CACMWAVE  
{
public:
	int open(IN char *fileName=NULL,IN int bRead=1);
	CAcmG7231(char *fName=NULL);
	virtual ~CAcmG7231();

};

#endif // !defined(AFX_ACMG7231_H__5CE932B8_CA5B_4A34_AAC3_B99C81BA092A__INCLUDED_)
