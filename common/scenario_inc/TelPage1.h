#if !defined(AFX_TELPAGE1_H__DF898F59_C9BB_468C_A7BC_F6CC01A06AC7__INCLUDED_)
#define AFX_TELPAGE1_H__DF898F59_C9BB_468C_A7BC_F6CC01A06AC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TelPage1.h : header file
//
#include "../resource.h"
/////////////////////////////////////////////////////////////////////////////
// CTelPage1 dialog

class CTelPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CTelPage1)

// Construction
public:
	CTelPage1();
	~CTelPage1();

// Dialog Data
	//{{AFX_DATA(CTelPage1)
	enum { IDD = IDD_TEL_PAGE_1 };
	CEdit	m_edDefCallDuration;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTelPage1)
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
	//{{AFX_MSG(CTelPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdDefcallduration();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELPAGE1_H__DF898F59_C9BB_468C_A7BC_F6CC01A06AC7__INCLUDED_)
