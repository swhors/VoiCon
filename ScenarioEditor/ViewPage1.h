#if !defined(AFX_VIEWPAGE1_H__C44696E3_4D80_40A9_A818_876F91F50223__INCLUDED_)
#define AFX_VIEWPAGE1_H__C44696E3_4D80_40A9_A818_876F91F50223__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewPage1 form view

#include "toolTipButton.h"
#include "afxwin.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CViewPage1 : public CFormView
{
protected:
	CViewPage1();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewPage1)

// Form Data
public:
	//{{AFX_DATA(CViewPage1)
	enum { IDD = IDD_VIEW_PAGE1 };
	CStatic			m_sttype;
	CStatic			m_stgo;
	CStatic			m_stfalsego;
	CStatic			m_stid;
	CToolTipButton	m_btnPage;
	CToolTipButton	m_btnHub;
	CToolTipButton	m_btnGoto;
	CToolTipButton	m_btnWatch;
	CToolTipButton	m_btnSpeak;
	CToolTipButton	m_btnRetrial;
	CToolTipButton	m_btnPlug;
	CToolTipButton	m_btnPhone;
	CToolTipButton	m_btnMath;
	CToolTipButton	m_btnIs;
	CToolTipButton	m_btnInit;
	CToolTipButton	m_btnEqual;
	CToolTipButton	m_btnDtmf;
	CToolTipButton	m_btnCircuit;
	CToolTipButton	m_btnArrow;
	CToolTipButton	m_btnAndor;
	CToolTipButton	m_btnCallErr;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewPage1)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewPage1();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewPage1)
	afx_msg void OnBtnInit();
	afx_msg void OnBtnAndor();
	afx_msg void OnBtnArrow();
	afx_msg void OnBtnCircuit();
	afx_msg void OnBtnDtmf();
	afx_msg void OnBtnEqual();
	afx_msg void OnBtnIs();
	afx_msg void OnBtnMath();
	afx_msg void OnBtnPhone();
	afx_msg void OnBtnPlug();
	afx_msg void OnBtnRetrial();
	afx_msg void OnBtnSpeak();
	afx_msg void OnBtnWatch();
	afx_msg LRESULT	OnUpdateClientInfo(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBtnGoto();
	afx_msg void OnBtnPage();
	afx_msg void OnBtnCallerr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnHub();
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWPAGE1_H__C44696E3_4D80_40A9_A818_876F91F50223__INCLUDED_)
