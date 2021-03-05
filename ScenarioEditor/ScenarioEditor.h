// ScenarioEditor.h : main header file for the SCENARIOEDITOR application
//

#if !defined(AFX_SCENARIOEDITOR_H__6ED6D2B4_DAAD_409A_8F78_E81021BE948A__INCLUDED_)
#define AFX_SCENARIOEDITOR_H__6ED6D2B4_DAAD_409A_8F78_E81021BE948A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "ScenarioEditorClipboardHandler.h"
#include "ViewPage2.h"
#include "ViewPage1.h"

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorApp:
// See ScenarioEditor.cpp for the implementation of this class
//

class CScenarioEditorApp : public CWinApp
{
public:
	CPoint m_endPoint;
	CPoint m_startPoint;
	UINT m_iSelectedItem;
	CScenarioEditorApp();
	CScenarioEditorClipboardHandler	m_clip;
	CViewPage2 * m_MainViewPage;
	CViewPage1 * m_IndexPage;
	DWORD		GetResourceID();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScenarioEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CScenarioEditorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENARIOEDITOR_H__6ED6D2B4_DAAD_409A_8F78_E81021BE948A__INCLUDED_)
