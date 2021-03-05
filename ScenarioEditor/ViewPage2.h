#if !defined(AFX_VIEWPAGE2_H__7056A651_C84F_4BEF_8644_951D99ADA4C2__INCLUDED_)
#define AFX_VIEWPAGE2_H__7056A651_C84F_4BEF_8644_951D99ADA4C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewPage2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewPage2 form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "ScenarioEditor1.h"	// Added by ClassView
#include "ScenarioEditorDoc.h"
#include "resource.h"

class CViewPage2 : public CFormView
{
protected:
	CViewPage2();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewPage2)

// Form Data
public:
	//{{AFX_DATA(CViewPage2)
	enum { IDD = IDD_VIEW_PAGE2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CPoint LineStart;
	CPoint LineEnd;
// Operations
public:
	int m_preVertSize;
	int m_preHorzSize;
	CScenarioEditorDoc	* GetDocument();
	CScenarioEditor		m_editor;
	int					m_screenResolutionX;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewPage2)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewPage2();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewPage2)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg LRESULT	OnUpdateItem(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWPAGE2_H__7056A651_C84F_4BEF_8644_951D99ADA4C2__INCLUDED_)
