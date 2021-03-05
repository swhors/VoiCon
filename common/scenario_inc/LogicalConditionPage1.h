#if !defined(AFX_LOGICALCONDITIONPAGE1_H__6BE02F9F_D9A6_4362_9D7D_34EF9F818667__INCLUDED_)
#define AFX_LOGICALCONDITIONPAGE1_H__6BE02F9F_D9A6_4362_9D7D_34EF9F818667__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogicalConditionPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogicalConditionPage1 dialog

class CLogicalConditionPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CLogicalConditionPage1)

// Construction
public:
	CLogicalConditionPage1();
	~CLogicalConditionPage1();

// Dialog Data
	//{{AFX_DATA(CLogicalConditionPage1)
	enum { IDD = IDD_LOGICALCON_PAGE_1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogicalConditionPage1)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogicalConditionPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGICALCONDITIONPAGE1_H__6BE02F9F_D9A6_4362_9D7D_34EF9F818667__INCLUDED_)
