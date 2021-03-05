#if !defined(AFX_SKINTREECTRL_H__C7978A5A_4F53_4027_AED4_E037FA8DCF33__INCLUDED_)
#define AFX_SKINTREECTRL_H__C7978A5A_4F53_4027_AED4_E037FA8DCF33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinTreeCtrl.h : header file
//

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CSkinTreeCtrl window

class CSkinTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CSkinTreeCtrl();

// Attributes
public:

protected:
	struct Color_Font
	{
		COLORREF color;
		LOGFONT  logfont;
	};
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont ;// Operations
	
	COLORREF	m_BkColor;
	CImageList	m_ImgList;
public:
	void Init( int x, int y, int w, int h, COLORREF bkcolor = RGB( 255, 255, 255 ) );
	void InitImageList( int w, int h );
	BOOL AddImage( char *bmpFile );
	void SetItemFont(HTREEITEM hItem, LOGFONT& logfont);
	void SetItemBold(HTREEITEM hItem, BOOL bBold);
	void SetItemColor(HTREEITEM hItem, COLORREF color);
	BOOL GetItemFont(HTREEITEM hItem, LOGFONT * plogfont);
	BOOL GetItemBold(HTREEITEM hItem);
	COLORREF GetItemColor(HTREEITEM hItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinTreeCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINTREECTRL_H__C7978A5A_4F53_4027_AED4_E037FA8DCF33__INCLUDED_)
