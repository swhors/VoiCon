#if !defined(AFX_SKINSLIDE_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_)
#define AFX_SKINSLIDE_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinSlide.h : header file
//
#define	SL_CHANGE_MIC_VAL	(WM_USER + 1234)
#define	SL_CHANGE_SPK_VAL	(WM_USER + 1235)

/////////////////////////////////////////////////////////////////////////////
// CSkinSlide window

class CSkinSlide : public CButton
{
// Construction
public:
	CSkinSlide();

// Attributes
protected:
	CBitmap* m_bmpHead;
	CBitmap* m_bmpBody;
	CBitmap* m_bmpTail;
	UINT	m_TransColor;

	int		m_Type;			//0 : Mic, 1:Spk
	int		m_nXPos;
	int		m_nYPos;
	int		m_nWidth;
	int		m_nHeight;

	int		m_HeadHeight;
	int		m_BodyHeight;
	int		m_TailHeight;
	int		m_BodyCnt;

	int		m_MinVal;
	int		m_MaxVal;

	int		m_CurY;
	int		m_TargetY;

	int		m_CurVal;
	int		m_OldVal;

	int		m_nHead;
	int		m_nBody;
	int		m_nTail;
	BOOL	m_bMoving;

// Operations
public:
	void SetType( int nType );
	void SetHeadBmp( char* HeadBmpFile, int nHeight );
	void SetBodyBmp( char* BodyBmpFile, int nHeight );
	void SetTailBmp( char* TailBmpFile, int nHeight );
	void SizeToContent(int x, int y, int nWidth, int nHeight, BOOL bRedraw);
	void SetRange( int MinVal, int MaxVal );
	void SetTransColor( UINT TransColor );
	int	GetValue();
	void SetValue( int Val );
	UINT GetTransColor();
private:
	void GetNofBody();
	int	GetValByPos( int yPos );
	int	GetPosByVal( int Val );
	void GetUnitCount( int *nHead, int *nBody, int *nTail );
	void DrawSlide();
	void SendValue();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSlide)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinSlide();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinSlide)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSLIDE_H__F430E083_5D7E_496B_8025_D02EA3FB5332__INCLUDED_)
