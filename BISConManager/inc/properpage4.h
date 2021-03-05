#if !defined(AFX_PROPERPAGE4_H__D97A78D6_DBEF_4FC9_B4A4_BDE73A3F74D9__INCLUDED_)
#define AFX_PROPERPAGE4_H__D97A78D6_DBEF_4FC9_B4A4_BDE73A3F74D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpage4.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPage4 dialog

class CProperPage4 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPage4)

// Construction
public:
	CProperPage4();
	~CProperPage4();

// Dialog Data
	//{{AFX_DATA(CProperPage4)
	enum { IDD = IDD_PROPPAGE_4 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPage4)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProperPage4)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGE4_H__D97A78D6_DBEF_4FC9_B4A4_BDE73A3F74D9__INCLUDED_)
