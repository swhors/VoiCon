#if !defined(AFX_PROPERPAGE5_H__D98E4F5E_6767_407A_95C0_BA3802CA74C6__INCLUDED_)
#define AFX_PROPERPAGE5_H__D98E4F5E_6767_407A_95C0_BA3802CA74C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpage5.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPage5 dialog

class CProperPage5 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPage5)

// Construction
public:
	CProperPage5();
	~CProperPage5();
	
	int	m_User_ID_Auth;
	int	m_HP_ID_Auth;
	int m_Tel_ID_Auth;
	
// Dialog Data
	//{{AFX_DATA(CProperPage5)
	enum { IDD = IDD_PROPPAGE_5 };
	BOOL	m_cIDNumber;
	BOOL	m_cHPNumber;
	BOOL	m_cTelNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPage5)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProperPage5)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGE5_H__D98E4F5E_6767_407A_95C0_BA3802CA74C6__INCLUDED_)
