#if !defined(AFX_CALLERRPAGE1_H__C8830F72_9A43_4F4B_BB0D_9A2B39A788EE__INCLUDED_)
#define AFX_CALLERRPAGE1_H__C8830F72_9A43_4F4B_BB0D_9A2B39A788EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// callerrpage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCallErrPage1 dialog

class CCallErrPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CCallErrPage1)

// Construction
public:
	CCallErrPage1();
	~CCallErrPage1();

// Dialog Data
	//{{AFX_DATA(CCallErrPage1)
	enum { IDD = IDD_CALLERR_PAGE_1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCallErrPage1)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCallErrPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALLERRPAGE1_H__C8830F72_9A43_4F4B_BB0D_9A2B39A788EE__INCLUDED_)
