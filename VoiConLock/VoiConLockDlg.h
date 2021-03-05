// VoiConLockDlg.h : header file
//

#if !defined(AFX_VOICONLOCKDLG_H__22D1797E_01C5_4F80_8948_6578555485A3__INCLUDED_)
#define AFX_VOICONLOCKDLG_H__22D1797E_01C5_4F80_8948_6578555485A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVoiConLockDlg dialog

class CVoiConLockDlg : public CDialog
{
// Construction
public:
	CVoiConLockDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVoiConLockDlg)
	enum { IDD = IDD_VOICONLOCK_DIALOG };
	CString	m_file_mac;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiConLockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVoiConLockDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGenerate();
	afx_msg void OnTimeBtn();
	afx_msg void OnMacVerify();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedSet();
	afx_msg void OnBtnSelf();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICONLOCKDLG_H__22D1797E_01C5_4F80_8948_6578555485A3__INCLUDED_)
