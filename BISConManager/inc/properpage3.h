#if !defined(AFX_PROPERPAGE3_H__B6A80AAF_F69F_4337_92CD_EB792D422AF4__INCLUDED_)
#define AFX_PROPERPAGE3_H__B6A80AAF_F69F_4337_92CD_EB792D422AF4__INCLUDED_

#include "ListBoxEx.h"
#include "afxwin.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpage3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPage3 dialog

class CProperPage3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPage3)

// Construction
public:
	int WriteToFileData(IN char * fFile, int iType);
	int ReadViewFileData(IN char *fFile, int iType);
	CProperPage3();
	~CProperPage3();

// Dialog Data
	//{{AFX_DATA(CProperPage3)
	enum { IDD = IDD_PROPPAGE_3 };
	CListBoxEx	m_cLanguage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPage3)
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
	//{{AFX_MSG(CProperPage3)
	virtual BOOL OnInitDialog();
	afx_msg void OnScefind();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	CEdit m_edLanguage;
public:
	CListBoxEx m_ListSce;
public:
	afx_msg void OnBnClickedTnNewLan();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGE3_H__B6A80AAF_F69F_4337_92CD_EB792D422AF4__INCLUDED_)
