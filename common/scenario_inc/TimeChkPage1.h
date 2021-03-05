#if !defined(AFX_TIMECHKPAGE1_H__F21EF64C_5689_488E_91F9_FA02C63B14C5__INCLUDED_)
#define AFX_TIMECHKPAGE1_H__F21EF64C_5689_488E_91F9_FA02C63B14C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeChkPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeChkPage1 dialog

class CTimeChkPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CTimeChkPage1)

// Construction
public:
	CTimeChkPage1();
	~CTimeChkPage1();

// Dialog Data
	//{{AFX_DATA(CTimeChkPage1)
	enum { IDD = IDD_TIMECHK_PAGE_1 };
	CEdit	m_edSFname;
	CEdit	m_edsgoto;
	CButton	m_chkspeday;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTimeChkPage1)
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
	//{{AFX_MSG(CTimeChkPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkspecialday();
	afx_msg void OnBtnSpOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMECHKPAGE1_H__F21EF64C_5689_488E_91F9_FA02C63B14C5__INCLUDED_)
