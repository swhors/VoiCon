#if !defined(AFX_ADDGRPDLG_H__65374E90_B4BF_4EEB_9F41_7397E88DBEB2__INCLUDED_)
#define AFX_ADDGRPDLG_H__65374E90_B4BF_4EEB_9F41_7397E88DBEB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddGrpDlg.h : header file
//

#include "SkinButton.h"

/////////////////////////////////////////////////////////////////////////////
// CAddGrpDlg dialog

class CAddGrpDlg : public CDialog
{
// Construction
public:
	CAddGrpDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddGrpDlg)
	enum { IDD = IDD_ADD_GRP_DIALOG };
	CEdit	m_edtGroup;
	CSkinButton	m_btnOK;
	CSkinButton	m_btnClose;
	CSkinButton	m_btnCancel;
	CString		m_szGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddGrpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	char	m_SkinDir[100];
	CBitmap m_bmpBkGnd;

	void	InitSkin();
	void	InitSkinHomeDir();
	BOOL	InitSkinBkGnd( char *BkGndFile );
	void	InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, 
							char *UpFile, char *DownFile, char *OverFile );

	// Generated message map functions
	//{{AFX_MSG(CAddGrpDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBtnClose();
	afx_msg void OnBtnCancel();
	afx_msg void OnBtnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDGRPDLG_H__65374E90_B4BF_4EEB_9F41_7397E88DBEB2__INCLUDED_)
