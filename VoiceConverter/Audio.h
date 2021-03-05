// Audio.h : main header file for the AUDIO application
//

#if !defined(AFX_AUDIO_H__A24D685A_566F_494D_BFB5_383C95940C66__INCLUDED_)
#define AFX_AUDIO_H__A24D685A_566F_494D_BFB5_383C95940C66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAudioApp:
// See Audio.cpp for the implementation of this class
//

class CAudioApp : public CWinApp
{
public:
	CAudioApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAudioApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIO_H__A24D685A_566F_494D_BFB5_383C95940C66__INCLUDED_)
