#if !defined(AFX_PROPERPAGEQ931_H__C022B02F_8326_4D98_AAA4_9B30484BE32E__INCLUDED_)
#define AFX_PROPERPAGEQ931_H__C022B02F_8326_4D98_AAA4_9B30484BE32E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProperPageQ931.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPageQ931 dialog

class CProperPageQ931 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPageQ931)

// Construction
public:
	CProperPageQ931();
	~CProperPageQ931();

// Dialog Data
	//{{AFX_DATA(CProperPageQ931)
	enum { IDD = IDD_PROPPAGE_Q931 };
	CButton	m_chk_fastStart;
	CButton	m_chk_h245Tunneling;
	int		m_ConnectTimeout;
	int		m_CallSignallingPort;
	int		m_ResponseTimeout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPageQ931)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProperPageQ931)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGEQ931_H__C022B02F_8326_4D98_AAA4_9B30484BE32E__INCLUDED_)
