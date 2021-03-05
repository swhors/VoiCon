#if !defined(AFX_GOTOPAGE1_H__781C2B95_666B_4DF5_A8E4_8E8371F0DD31__INCLUDED_)
#define AFX_GOTOPAGE1_H__781C2B95_666B_4DF5_A8E4_8E8371F0DD31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GotoPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGotoPage1 dialog

class CGotoPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CGotoPage1)

// Construction
public:
	CGotoPage1();
	~CGotoPage1();

// Dialog Data
	//{{AFX_DATA(CGotoPage1)
	enum { IDD = IDD_GOTO_PAGE1 };
	CEdit	m_edGoto;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGotoPage1)
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
	//{{AFX_MSG(CGotoPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTOPAGE1_H__781C2B95_666B_4DF5_A8E4_8E8371F0DD31__INCLUDED_)
