#if !defined(AFX_UBICONF_H__E0BB877F_E24F_4AB4_8ED1_8A25931596F9__INCLUDED_)
#define AFX_UBICONF_H__E0BB877F_E24F_4AB4_8ED1_8A25931596F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ubiconf.h : header file
//

#include "SkinButton.h"
#include "afxwin.h"
#include "afxcmn.h"


/////////////////////////////////////////////////////////////////////////////
// CUBIConf dialog

class CUBIConf : public CDialog
{
// Construction
public:
	void EnableG7231Item(BOOL bEnable=TRUE);
	void WriteSetting();
    char	m_SkinDir[100];
    CBitmap m_bmpBkGnd;

	CUBIConf(CWnd* pParent = NULL);   // standard constructor

	void InitSkinBtn(CSkinButton & Btn, int x, int y, int w, int h, CString & FileName);
	BOOL InitSkinBkGnd(char * BkGndFile);
	void loadSetting();
	void InitSkinHomeDir();
	void InitSkin();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
// Dialog Data
	//{{AFX_DATA(CUBIConf)
	enum { IDD = IDD_UBI_CONF1 };
	CSkinButton	m_exit;
	CSkinButton	m_btn_cancel;
	CSkinButton	m_btn_ok;
	CIPAddressCtrl	m_GKIP;
	CEdit	m_edResponseTimeOut;
	CEdit	m_edRasPort;
	CEdit	m_edRasE164;
	CEdit	m_edRasH323ID;
	CEdit	m_edQ931Port;
	CEdit	m_edMaxAl;
	CEdit	m_ed_gkPort;
	CButton	m_chkTunneling;
	CButton	m_chkSilenceSup;
	CButton	m_chkG7231;
	CButton	m_chkG711Ulaw;
	CButton	m_chkDTMF;
	CButton	m_chkFastStart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUBIConf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUBIConf)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnChkG7231();
	afx_msg void OnChkFaststart();
	afx_msg void OnChkTunneling();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UBICONF_H__E0BB877F_E24F_4AB4_8ED1_8A25931596F9__INCLUDED_)
