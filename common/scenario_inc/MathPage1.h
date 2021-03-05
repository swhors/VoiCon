#if !defined(AFX_MATHPAGE1_H__A06B803E_FB6F_47ED_B8EC_D8ADDCC33986__INCLUDED_)
#define AFX_MATHPAGE1_H__A06B803E_FB6F_47ED_B8EC_D8ADDCC33986__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MathPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMathPage1 dialog

class CMathPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CMathPage1)

// Construction
public:
	CMathPage1();
	~CMathPage1();

// Dialog Data
	//{{AFX_DATA(CMathPage1)
	enum { IDD = IDD_MATH_PAGE_1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMathPage1)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMathPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATHPAGE1_H__A06B803E_FB6F_47ED_B8EC_D8ADDCC33986__INCLUDED_)
