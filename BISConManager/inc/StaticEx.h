/*******************************************/
/*                                         */
/* - title : StaticEx.h                    */
/* - description : Static Extends          */
/* - author : you !                        */
/* - date : 2002-09-13                     */
/*                                         */
/*******************************************/
#if !defined(__STATIC__H__)
#define __STATIC__H__


class CStaticEx : public CStatic
{
/* Construction */
public:
	CStaticEx();
	virtual ~CStaticEx();

/* Attributes */
public:
	COLORREF	m_textColor;
	CBrush		m_backBrush;
	COLORREF	m_backColor;
	
/* Operations */
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticEx)
	//}}AFX_VIRTUAL

/* Implementation */
public:
	void SetBitmap(UINT nID, BOOL bStretch=FALSE, COLORREF transColor=::GetSysColor(COLOR_BTNFACE));
	void SetBkColor(COLORREF Color);
	void SetTextColor(COLORREF Color);
	void DrawTransparent(CDC*pDC, int x, int y, CBitmap& orgBMP, COLORREF crColor);

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticEx)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif