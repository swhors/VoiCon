#if !defined(AFX_PROPERPAGE2_H__31971C2B_9856_43F9_BEB0_8EB058A5A702__INCLUDED_)
#define AFX_PROPERPAGE2_H__31971C2B_9856_43F9_BEB0_8EB058A5A702__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpage2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPage2 dialog

class CProperPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPage2)

// Construction
public:
	CProperPage2();
	~CProperPage2();

// Dialog Data
	//{{AFX_DATA(CProperPage2)
	enum { IDD = IDD_PROPPAGE_2 };
	CIPAddressCtrl	m_MulticastIP;
	CIPAddressCtrl	m_cGKIP;
	int		m_cGKPort;
	int		m_RasPort;
	int		m_MulticastPort;
	int		m_ResponseTimeOut;
	BOOL	m_OverLapSending;
	int		m_maxFail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPage2)
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
	//{{AFX_MSG(CProperPage2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGE2_H__31971C2B_9856_43F9_BEB0_8EB058A5A702__INCLUDED_)
