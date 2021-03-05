#if !defined(AFX_WEBFORMVIEW_H__5DB1C64D_61A1_4A4C_9745_45ECF57FD413__INCLUDED_)
#define AFX_WEBFORMVIEW_H__5DB1C64D_61A1_4A4C_9745_45ECF57FD413__INCLUDED_

#include "ScenarioEditor1.h"
#include "ScenarioEditorClipboardHandler.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWebFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CTestWeb1Ctrl;

class CWebFormView : public CFormView
{
//protected:
public:
	CWebFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWebFormView)
	CScenarioEditor					m_editor;
	CPoint							LineStart;
	CPoint							LineEnd;
	CPoint							m_endPoint;
	CPoint							m_startPoint;
	UINT							m_iSelectedItem;
	int								m_screenResolutionX;
	CScenarioEditorClipboardHandler	m_clip;
	// Form Data
public:
	//{{AFX_DATA(CWebFormView)
	enum { IDD = IDD_DLG_FORMVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CTestWeb1Ctrl	*m_pDoc;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebFormView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnDraw(CDC* pDC);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
//protected:
public:
	int m_preVertSize;
	int m_preHorzSize;
	virtual ~CWebFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// Generated message map functions
	//{{AFX_MSG(CWebFormView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT	OnUpdateItem(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBFORMVIEW_H__5DB1C64D_61A1_4A4C_9745_45ECF57FD413__INCLUDED_)
