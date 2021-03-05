#if !defined(AFX_SMSMAINDLG_H__F1800924_C894_4684_AB76_F4577D66D644__INCLUDED_)
#define AFX_SMSMAINDLG_H__F1800924_C894_4684_AB76_F4577D66D644__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SmsMainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSmsMainDlg dialog
#include "SkinButton.h"
#include "EditEx.h"
#include "SmsMainDlg.h"

class CSmsMainDlg : public CDialog
{
// Construction
public:
	CSmsMainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSmsMainDlg)
	enum { IDD = IDD_SMS_DIALOG };
	CSkinButton	m_btnSymbol;
	CEditEx	m_edtTo;
	CEditEx	m_edtFrom;
	CSkinButton	m_btnSend;
	CEditEx	m_edtMesg;
	CSkinButton	m_btnClose;
	CString	m_strFrom;
	CString	m_strTo;
	CString	m_strMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char	m_SkinDir[100];
	CBitmap m_bmpBkGnd;	
	
	void	InitSkin();
	void	InitSkinHomeDir();
	BOOL	InitSkinBkGnd( char *BkGndFile );
	void	InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, CString &FileName );	
	// Generated message map functions
	//{{AFX_MSG(CSmsMainDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnClose();
	afx_msg void OnBtnSend();
	afx_msg void OnBtnSymbol();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
extern CSmsMainDlg *pSmsDlg;
#endif // !defined(AFX_SMSMAINDLG_H__F1800924_C894_4684_AB76_F4577D66D644__INCLUDED_)
