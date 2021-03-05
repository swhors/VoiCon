#if !defined(AFX_PLUGPAGE1_H__7111C6B5_E087_4422_B009_45E37C43F7F2__INCLUDED_)
#define AFX_PLUGPAGE1_H__7111C6B5_E087_4422_B009_45E37C43F7F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlugPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlugPage1 dialog

class CPlugPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPlugPage1)

// Construction
public:
	CPlugPage1();
	~CPlugPage1();

// Dialog Data
	//{{AFX_DATA(CPlugPage1)
	enum { IDD = IDD_PLUG_PAGE_1 };
	CEdit	m_edFunction;
	CEdit	m_edDllName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPlugPage1)
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
	//{{AFX_MSG(CPlugPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGPAGE1_H__7111C6B5_E087_4422_B009_45E37C43F7F2__INCLUDED_)
