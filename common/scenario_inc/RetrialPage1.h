#if !defined(AFX_RETRIALPAGE1_H__0D79751D_AC45_45F6_AF41_BD84BF4637EE__INCLUDED_)
#define AFX_RETRIALPAGE1_H__0D79751D_AC45_45F6_AF41_BD84BF4637EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RetrialPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRetrialPage1 dialog

class CRetrialPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CRetrialPage1)

// Construction
public:
	void init_cb(IN int iSelItem=-1);
	CRetrialPage1();
	~CRetrialPage1();

// Dialog Data
	//{{AFX_DATA(CRetrialPage1)
	enum { IDD = IDD_RETRIAL_PAGE_1 };
	CEdit	m_edValue;
	CComboBox	m_cbCommand;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRetrialPage1)
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
	//{{AFX_MSG(CRetrialPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboCommand();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RETRIALPAGE1_H__0D79751D_AC45_45F6_AF41_BD84BF4637EE__INCLUDED_)
