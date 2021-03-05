#if !defined(AFX_SKINBUTTON_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_)
#define AFX_SKINBUTTON_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkinButton window

class CSkinButton : public CButton
{
// Construction
public:
	CSkinButton();

// Attributes
protected:
	CBitmap* m_bmpBtnUp;
	CBitmap* m_bmpBtnOv;
	CBitmap* m_bmpBtnDn;

	int		m_nXPos;
	int		m_nYPos;
	int		m_nWidth;
	int		m_nHeight;

	BOOL	m_bIsPointed;
	int		m_btnStat;
	UINT	m_TransColor;

	enum	{ ST_BTN_UP, ST_BTN_OVER, ST_BTN_DOWN };
	
// Operations
public:
	void LoadBitmaps( CBitmap* BtnUp, CBitmap* BtnDn = NULL, CBitmap* BtnOv = NULL);
	void LoadBitmapFiles( char* UpBmpFile, char *DnBmpFile = NULL, char* OvBmpFile = NULL);
	void SizeToContent(int x, int y, int nWidth, int nHeight, BOOL bRedraw);
	void SetTransColor( UINT TransColor );
	UINT GetTransColor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINBUTTON_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_)
