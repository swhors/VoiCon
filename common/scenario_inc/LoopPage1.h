#if !defined(AFX_LOOPPAGE1_H__6F4648D9_99C9_439F_B8EE_A005909BDDA9__INCLUDED_)
#define AFX_LOOPPAGE1_H__6F4648D9_99C9_439F_B8EE_A005909BDDA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoopPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoopPage1 dialog

class CLoopPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CLoopPage1)

// Construction
public:
	void EnableRdButton(unsigned int iID=IDC_RD11);
	CLoopPage1();
	~CLoopPage1();

// Dialog Data
	//{{AFX_DATA(CLoopPage1)
	enum { IDD = IDD_LOOP_PAGE_1 };
	CButton	m_chkCaller;
	CButton	m_chkCallee;
	CButton	m_rd15;
	CButton	m_rd14;
	CButton	m_rd13;
	CButton	m_rd12;
	CButton	m_rd11;
	CEdit	m_edNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLoopPage1)
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
	//{{AFX_MSG(CLoopPage1)
	afx_msg void OnRd13();
	afx_msg void OnRd12();
	afx_msg void OnRd11();
	afx_msg void OnRd14();
	afx_msg void OnRd15();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdNum();
	afx_msg void OnChkCaller();
	afx_msg void OnChkCallee();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOOPPAGE1_H__6F4648D9_99C9_439F_B8EE_A005909BDDA9__INCLUDED_)
