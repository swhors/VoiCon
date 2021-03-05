// AcmG711u.h: interface for the CAcmG711u class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACMG711U_H__59B72B99_F381_4911_9F3E_E356CC9F0BE2__INCLUDED_)
#define AFX_ACMG711U_H__59B72B99_F381_4911_9F3E_E356CC9F0BE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ACMWAVE.h"

class CAcmG711u : public CACMWAVE  
{
public:
	int open(IN char *fileName=NULL,IN int bRead=1);
	CAcmG711u(char *fName=NULL);
	virtual ~CAcmG711u();
};

#endif // !defined(AFX_ACMG711U_H__59B72B99_F381_4911_9F3E_E356CC9F0BE2__INCLUDED_)
