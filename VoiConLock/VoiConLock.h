// VoiConLock.h : main header file for the VOICONLOCK application
//

#if !defined(AFX_VOICONLOCK_H__AD493A6C_ABB6_488C_9BD1_253EE7226714__INCLUDED_)
#define AFX_VOICONLOCK_H__AD493A6C_ABB6_488C_9BD1_253EE7226714__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVoiConLockApp:
// See VoiConLock.cpp for the implementation of this class
//

class CVoiConLockApp : public CWinApp
{
public:
	CVoiConLockApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiConLockApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVoiConLockApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICONLOCK_H__AD493A6C_ABB6_488C_9BD1_253EE7226714__INCLUDED_)
