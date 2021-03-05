#if !defined(AFX_STATEVIEW_H__D32B4B68_55E8_44CE_902E_06719D4C6605__INCLUDED_)
#define AFX_STATEVIEW_H__D32B4B68_55E8_44CE_902E_06719D4C6605__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StateView.h : header file
//

#include "StaticEx.h"
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// CStateView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CStateView : public CFormView
{
protected:
	CStateView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CStateView)

	int m_bUpdate;
// Form Data
public:
	void	StateChange(int channel, int state , CString sE164, CString dE164, int sCenarioStep=0);
	int m_nActive;
	RECT	ctrl_rect, ctrl_rect2, ctrl_rect3, ctrl_rect4;
	int m_nTotalCall;
	//{{AFX_DATA(CStateView)
	enum { IDD = IDD_STATEDLG };
	CEdit		m_edCurCount;
	CStatic		m_stTotalTitle;
	CEdit		m_edTotalCount;
	CStaticEx	m_cCurrentTitle;
	CListCtrl	m_cCallListCtrl;
	//}}AFX_DATA

// Attributes
public:
	CImageList	m_LargeImageList;
	BOOL bInited;
// Operations

public:
	void SetInitView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStateView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CStateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CStateView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnInfoTip( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnPopCallinfo();
	afx_msg void OnPopReset();
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRclickList2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void UpdateCurrentCall();
	void init_count_state();
	int m_iListSelectedItem;
	void pop_menu(POSITION &pos);
	CToolTipCtrl m_StateToolTip;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATEVIEW_H__D32B4B68_55E8_44CE_902E_06719D4C6605__INCLUDED_)
