#if !defined(AFX_CALENDARDLG_H__C8A8C9B4_C20A_4FB3_8590_4531DB51D2C1__INCLUDED_)
#define AFX_CALENDARDLG_H__C8A8C9B4_C20A_4FB3_8590_4531DB51D2C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalendarDlg.h : header file
//

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCalendarDlg dialog

class CCalendarDlg : public CDialog
{
// Construction
public:
	//CTime refDateTime;
	CString			selectedTime;
	CCalendarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalendarDlg)
	enum { IDD = IDD_CALENDAR };
	CDateTimeCtrl	m_time;
	CMonthCalCtrl	m_calendar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalendarDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALENDARDLG_H__C8A8C9B4_C20A_4FB3_8590_4531DB51D2C1__INCLUDED_)
