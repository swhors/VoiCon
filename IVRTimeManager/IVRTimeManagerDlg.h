// IVRTimeManagerDlg.h : header file
//

#if !defined(AFX_IVRTIMEMANAGERDLG_H__8EDE7F06_5775_4A0E_9329_2BA11B96A0F3__INCLUDED_)
#define AFX_IVRTIMEMANAGERDLG_H__8EDE7F06_5775_4A0E_9329_2BA11B96A0F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CIVRTimeManagerDlg dialog

class CIVRTimeManagerDlg : public CDialog
{
// Construction
public:
	void pReLoad();
	CIVRTimeManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIVRTimeManagerDlg)
	enum { IDD = IDD_IVRTIMEMANAGER_DIALOG };
	CEdit	m_ed_pre2;
	CEdit	m_ed_pre;
	CString	m_start;
	CString	m_end;
	CString	m_total;
	CString	m_PreInfo;
	CString	m_PreInfo2;
	CString	m_start2;
	CString	m_end2;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIVRTimeManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CString csBegin;
	CString csEnd;
	// Generated message map functions
	//{{AFX_MSG(CIVRTimeManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnUpdate();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnEnd();
	afx_msg void OnAbout();
	afx_msg void OnBtnStart2();
	afx_msg void OnBtnEnd2();
	afx_msg void OnUpdateDaily();
	afx_msg void OnOpenWork();
	afx_msg void OnSpeOpen();
	afx_msg void OnNewDaily();
	afx_msg void OnBtnNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IVRTIMEMANAGERDLG_H__8EDE7F06_5775_4A0E_9329_2BA11B96A0F3__INCLUDED_)
