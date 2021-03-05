// AcmG711a.h: interface for the CAcmG711a class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACMG711A_H__3668C8E9_0B92_4765_9EB1_AD55A6876CA2__INCLUDED_)
#define AFX_ACMG711A_H__3668C8E9_0B92_4765_9EB1_AD55A6876CA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ACMWAVE.h"

class CAcmG711a : public CACMWAVE  
{
public:
	int open(IN char *fileName=NULL,IN int bRead=1);
	CAcmG711a(char *fName=NULL);
	virtual ~CAcmG711a();

};

#endif // !defined(AFX_ACMG711A_H__3668C8E9_0B92_4765_9EB1_AD55A6876CA2__INCLUDED_)
