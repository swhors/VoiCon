#if !defined(AFX_PROPERPAGE7_H__598CDFAD_C2FE_484C_B551_9A6B36F15F9F__INCLUDED_)
#define AFX_PROPERPAGE7_H__598CDFAD_C2FE_484C_B551_9A6B36F15F9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpage7.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPage7 dialog

class CProperPage7 : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPage7)

// Construction
public:
	CProperPage7();
	~CProperPage7();

// Dialog Data
	//{{AFX_DATA(CProperPage7)
	enum { IDD = IDD_PROPPAGE_7 };
	CButton	m_chk_h245Tunneling;
	CButton	m_chk_fastStart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPage7)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProperPage7)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGE7_H__598CDFAD_C2FE_484C_B551_9A6B36F15F9F__INCLUDED_)
