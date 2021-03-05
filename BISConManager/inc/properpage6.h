#if !defined(AFX_PROPERPAGE6_H__1F87950D_69AE_4BF7_BA78_9EDE35848BDF__INCLUDED_)
#define AFX_PROPERPAGE6_H__1F87950D_69AE_4BF7_BA78_9EDE35848BDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpage6.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPage6 dialog

class CProperPage6 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPage6)

// Construction
public:
	CProperPage6();
	~CProperPage6();

// Dialog Data
	//{{AFX_DATA(CProperPage6)
	enum { IDD = IDD_PROPPAGE_6 };
	CButton	m_chk_terminal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPage6)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProperPage6)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkAutoTerminal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGE6_H__1F87950D_69AE_4BF7_BA78_9EDE35848BDF__INCLUDED_)
