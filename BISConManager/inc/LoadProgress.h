#if !defined(AFX_LOADPROGRESS_H__96C7C0B6_BEE4_450D_970D_F0563EFF03AE__INCLUDED_)
#define AFX_LOADPROGRESS_H__96C7C0B6_BEE4_450D_970D_F0563EFF03AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadProgress.h : header file
//
#define MAX_PCHK_COUNT	120
/////////////////////////////////////////////////////////////////////////////
// CLoadProgress dialog

class CLoadProgress : public CDialog
{
// Construction
public:
	CLoadProgress(CWnd* pParent = NULL);   // standard constructor
	BOOL	bStopPaint;
	HANDLE	hThread;
	int		pChkCount;
	void	destroy();
// Dialog Data
	//{{AFX_DATA(CLoadProgress)
	enum { IDD = IDD_DLG_PROC };
	CProgressCtrl	m_progre;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadProgress)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADPROGRESS_H__96C7C0B6_BEE4_450D_970D_F0563EFF03AE__INCLUDED_)
