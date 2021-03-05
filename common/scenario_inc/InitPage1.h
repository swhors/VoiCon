#if !defined(AFX_INITPAGE1_H__1D437AAD_E54B_4EF1_9942_8DAD3EC17A0F__INCLUDED_)
#define AFX_INITPAGE1_H__1D437AAD_E54B_4EF1_9942_8DAD3EC17A0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InitPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInitPage1 dialog

class CInitPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPage1)

// Construction
public:
	CInitPage1();
	~CInitPage1();

// Dialog Data
	//{{AFX_DATA(CInitPage1)
	enum { IDD = IDD_INIT_PAGE_1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPage1)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INITPAGE1_H__1D437AAD_E54B_4EF1_9942_8DAD3EC17A0F__INCLUDED_)
