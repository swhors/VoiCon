#if !defined(AFX_PAGEPAGE1_H__4D0E9074_3789_40E0_87D3_ABF9FCA68BF0__INCLUDED_)
#define AFX_PAGEPAGE1_H__4D0E9074_3789_40E0_87D3_ABF9FCA68BF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PagePage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPagePage1 dialog

class CPagePage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePage1)

// Construction
public:
	CPagePage1();
	~CPagePage1();

// Dialog Data
	//{{AFX_DATA(CPagePage1)
	enum { IDD = IDD_PAGE_PAGE1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePage1)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPAGE1_H__4D0E9074_3789_40E0_87D3_ABF9FCA68BF0__INCLUDED_)
