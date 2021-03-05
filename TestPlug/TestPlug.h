// TestPlug.h : main header file for the TESTPLUG DLL
//

#if !defined(AFX_TESTPLUG_H__0BBDF8A1_6247_483F_8BA0_6D8950CBF37D__INCLUDED_)
#define AFX_TESTPLUG_H__0BBDF8A1_6247_483F_8BA0_6D8950CBF37D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestPlugApp
// See TestPlug.cpp for the implementation of this class
//

class CTestPlugApp : public CWinApp
{
public:
	CTestPlugApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestPlugApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTestPlugApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTPLUG_H__0BBDF8A1_6247_483F_8BA0_6D8950CBF37D__INCLUDED_)
