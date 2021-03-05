#if !defined(AFX_TIMEDLG1_H__286CAB5C_BDEA_43A1_AFEB_484F1941AE89__INCLUDED_)
#define AFX_TIMEDLG1_H__286CAB5C_BDEA_43A1_AFEB_484F1941AE89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeDlg1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg1 dialog

class CTimeDlg1 : public CDialog
{
// Construction
public:
	CString csTime;
	CString csCalendar;
	CTimeDlg1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimeDlg1)
	enum { IDD = IDD_DLG_TIME };
	CDateTimeCtrl	m_calendar;
	CDateTimeCtrl	m_time;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeDlg1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeDlg1)
	virtual void OnOK();
	afx_msg void OnCloseupCalendar(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDLG1_H__286CAB5C_BDEA_43A1_AFEB_484F1941AE89__INCLUDED_)
