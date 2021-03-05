#if !defined(AFX_CONDITIONPAGE1_H__CBBEC698_9512_4812_B652_910A2F585DF0__INCLUDED_)
#define AFX_CONDITIONPAGE1_H__CBBEC698_9512_4812_B652_910A2F585DF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConditionPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConditionPage1 dialog

class CConditionPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CConditionPage1)

// Construction
public:
	CConditionPage1();
	~CConditionPage1();

// Dialog Data
	//{{AFX_DATA(CConditionPage1)
	enum { IDD = IDD_CONDITION_PAGE_1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConditionPage1)
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
	//{{AFX_MSG(CConditionPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDITIONPAGE1_H__CBBEC698_9512_4812_B652_910A2F585DF0__INCLUDED_)
