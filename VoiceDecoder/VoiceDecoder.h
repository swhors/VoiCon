// VoiceDecoder.h : main header file for the VOICEDECODER application
//

#if !defined(AFX_VOICEDECODER_H__223E60E5_BFA0_4394_B283_B48052A71B77__INCLUDED_)
#define AFX_VOICEDECODER_H__223E60E5_BFA0_4394_B283_B48052A71B77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVoiceDecoderApp:
// See VoiceDecoder.cpp for the implementation of this class
//

class CVoiceDecoderApp : public CWinApp
{
public:
	CVoiceDecoderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceDecoderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVoiceDecoderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICEDECODER_H__223E60E5_BFA0_4394_B283_B48052A71B77__INCLUDED_)
