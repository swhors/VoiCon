#if !defined(AFX_LISTBOXEX_H__EB52DD6E_08A5_44A0_9E66_165B67B1F7FA__INCLUDED_)
#define AFX_LISTBOXEX_H__EB52DD6E_08A5_44A0_9E66_165B67B1F7FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxEx.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CListBoxEx window

class CListBoxEx : public CListBox
{
// Construction
public:
	CListBoxEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	void MoveListString();
	BOOL bLButtonPressed;
	char	sSelected[256];
	CString csSelected;
	int iSelected;
	virtual ~CListBoxEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxEx)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRemoveItem();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
//	CItemStack * pSelItem;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXEX_H__EB52DD6E_08A5_44A0_9E66_165B67B1F7FA__INCLUDED_)
