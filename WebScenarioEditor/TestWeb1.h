#if !defined(AFX_TESTWEB1_H__5723FCC9_8AC6_454B_841A_A8E774A65AC8__INCLUDED_)
#define AFX_TESTWEB1_H__5723FCC9_8AC6_454B_841A_A8E774A65AC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TestWeb1.h : main header file for TESTWEB1.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestWeb1App : See TestWeb1.cpp for implementation.

class CTestWeb1App : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTWEB1_H__5723FCC9_8AC6_454B_841A_A8E774A65AC8__INCLUDED)
