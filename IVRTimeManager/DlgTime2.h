#if !defined(AFX_DLGTIME2_H__94F89AF7_A4FE_4919_A584_ADFA02C65432__INCLUDED_)
#define AFX_DLGTIME2_H__94F89AF7_A4FE_4919_A584_ADFA02C65432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTime2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTime2 dialog

class CDlgTime2 : public CDialog
{
// Construction
public:
	int m_minute;
	int m_hour;
	int m_day;
	int m_month;
	int m_year;
	CDlgTime2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTime2)
	enum { IDD = IDD_DLG_TIME2 };
	CEdit	m_edYear;
	CEdit	m_edDay;
	CEdit	m_edMonth;
	CEdit	m_edMinute;
	CEdit	m_edHour;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTime2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTime2)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeYear();
	afx_msg void OnChangeMonth();
	afx_msg void OnChangeDay();
	afx_msg void OnChangeHour();
	afx_msg void OnChangeMinute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTIME2_H__94F89AF7_A4FE_4919_A584_ADFA02C65432__INCLUDED_)
