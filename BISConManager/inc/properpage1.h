#if !defined(AFX_PROPERPAGE1_H__9F54BB70_EE49_447F_9DEF_B55F2F5CA4AC__INCLUDED_)
#define AFX_PROPERPAGE1_H__9F54BB70_EE49_447F_9DEF_B55F2F5CA4AC__INCLUDED_

#include "ListBoxEx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPage1 dialog

class CProperPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPage1)

// Construction
public:
	CProperPage1(const char* lpszTitle=NULL);
	~CProperPage1();

// Dialog Data
	//{{AFX_DATA(CProperPage1)
	enum { IDD = IDD_PROPPAGE_1 };
	CListBoxEx	m_lbE164;
	CEdit	m_edNewE164;
	int		m_nMaxCall;
	CString	m_sH323ID;
	CString	m_sNewE164;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPage1)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProperPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGE1_H__9F54BB70_EE49_447F_9DEF_B55F2F5CA4AC__INCLUDED_)
