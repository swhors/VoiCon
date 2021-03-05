#if !defined(AFX_ISPAGE1_H__14EEAF78_38E8_4E0D_9F5B_7471C68B9CE1__INCLUDED_)
#define AFX_ISPAGE1_H__14EEAF78_38E8_4E0D_9F5B_7471C68B9CE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IsPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIsPage1 dialog

class CIsPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CIsPage1)

// Construction
public:
	void SetValPck(int iOpt=0);
	void init_cb(IN int iSelItem,IN CComboBox * pCombo,int type=1);
	CIsPage1();
	~CIsPage1();

// Dialog Data
	//{{AFX_DATA(CIsPage1)
	enum { IDD = IDD_IS_PAGE_1 };
	CButton	m_chkUInput;
	CButton	m_chkSrcBuffer;
	CComboBox	m_cbSrcCtrl;
	CComboBox	m_cbInput;
	CEdit	m_edValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIsPage1)
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
	//{{AFX_MSG(CIsPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditchangeComboInput();
	afx_msg void OnChangeEdValue();
	afx_msg void OnSelchangeComboInput();
	afx_msg void OnChk01();
	afx_msg void OnChk02();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISPAGE1_H__14EEAF78_38E8_4E0D_9F5B_7471C68B9CE1__INCLUDED_)
