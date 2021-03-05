#if !defined(AFX_PLAYPAGE1_H__45AFF93E_246C_4BBF_BD03_55419D1ECB89__INCLUDED_)
#define AFX_PLAYPAGE1_H__45AFF93E_246C_4BBF_BD03_55419D1ECB89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlayPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlayPage1 dialog

class CPlayPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPlayPage1)

// Construction
public:
	CPlayPage1();
	~CPlayPage1();

// Dialog Data
	//{{AFX_DATA(CPlayPage1)
	enum { IDD = IDD_PLAY_PAGE_1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPlayPage1)
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
	//{{AFX_MSG(CPlayPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYPAGE1_H__45AFF93E_246C_4BBF_BD03_55419D1ECB89__INCLUDED_)
