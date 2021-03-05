#if !defined(AFX_ADDLISTDLG_H__EB30FFC6_AEFB_4FB3_A709_D4A0353A221B__INCLUDED_)
#define AFX_ADDLISTDLG_H__EB30FFC6_AEFB_4FB3_A709_D4A0353A221B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddListDlg.h : header file
//

#include "SkinButton.h"
#include "PhoneBookDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAddListDlg dialog

class CAddListDlg : public CDialog
{
// Construction
public:
	CAddListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddListDlg)
	enum { IDD = IDD_ADD_LIST_DIALOG };
	CStatic	m_lblGroup;
	CEdit	m_edtUbiPhone;
	CEdit	m_edtPerson;
	CEdit	m_edtHomePhone;
	CEdit	m_edtHandPhone;
	CSkinButton	m_btnOK;
	CSkinButton	m_btnClose;
	CSkinButton	m_btnCancel;
	CString	m_szGroup;
	CString	m_szHandPhoneNo;
	CString	m_szHomePhoneNo;
	CString	m_szPerson;
	CString	m_szUbiPhoneNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	void	SetGroup( char *szGroup );
	void	SetData( char *szGroup, char *szPerson, char szPhoneNo[][PHONE_NUMBER_SIZE+1] );

protected:
	char	m_SkinDir[100];
	CBitmap m_bmpBkGnd;

	char	m_szSelGroup[GROUP_NAME_SIZE+1];
	char	m_szSelPerson[PERSON_NAME_SIZE+1];
	char	m_szSelPhoneNo[3][PHONE_NUMBER_SIZE+1];

	void	InitSkin();
	void	InitSkinHomeDir();
	BOOL	InitSkinBkGnd( char *BkGndFile );
	void	InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, 
							char *UpFile, char *DownFile, char *OverFile );

	// Generated message map functions
	//{{AFX_MSG(CAddListDlg)
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

#endif // !defined(AFX_ADDLISTDLG_H__EB30FFC6_AEFB_4FB3_A709_D4A0353A221B__INCLUDED_)
