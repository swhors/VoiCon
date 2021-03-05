#if !defined(AFX_SKINLABEL_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_)
#define AFX_SKINLABEL_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinLabel.h : header file
//

#include	"SkinFont.h"

typedef struct  {
	int		size;
	char	FontName[30];
} t_Font;

/////////////////////////////////////////////////////////////////////////////
// CSkinLabel window

class CSkinLabel : public CButton
{
// Construction
public:
	CSkinLabel();

// Attributes
protected:
	int		m_nXPos;
	int		m_nYPos;
	int		m_nWidth;
	int		m_nHeight;
	int		m_Align;		
	int		m_MaxLength;

	CString	m_Str;
	CSkinFont	*m_SkinFont;
	t_Font		*m_TextFont;
// Operations
public:
	void SizeToContent(int x, int y, int nWidth, int nHeight, BOOL bRedraw);
    void AddInt(int i);
	void SetLabel( CString Str );
	void AddString( CString Str );
	void DelChar();
	void SetTextAlign( UINT Align );
	void SetSkinFont( CSkinFont *SkinFont );
	void SetSkinFont( t_Font *SkinFont );
	void TextOut( CDC *pDc );
	void SetMaxLength( int len );

	CString GetLabel();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinLabel)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinLabel)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINLABEL_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_)
