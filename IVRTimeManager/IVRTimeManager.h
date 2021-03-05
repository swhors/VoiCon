// IVRTimeManager.h : main header file for the IVRTIMEMANAGER application
//

#if !defined(AFX_IVRTIMEMANAGER_H__D2C749CC_1229_4611_895C_D75F69956D4C__INCLUDED_)
#define AFX_IVRTIMEMANAGER_H__D2C749CC_1229_4611_895C_D75F69956D4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIVRTimeManagerApp:
// See IVRTimeManager.cpp for the implementation of this class
//

class CIVRTimeManagerApp : public CWinApp
{
public:
	CIVRTimeManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIVRTimeManagerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIVRTimeManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IVRTIMEMANAGER_H__D2C749CC_1229_4611_895C_D75F69956D4C__INCLUDED_)
