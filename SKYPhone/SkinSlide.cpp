// SkinSlide.cpp : implementation file
//

#include "stdafx.h"
#include "SkinSlide.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinSlide

CSkinSlide::CSkinSlide()
{
	m_Type = 0;

	m_bmpHead = NULL;
	m_bmpBody = NULL;
	m_bmpTail = NULL;

	m_HeadHeight = 0;
	m_BodyHeight = 0;
	m_TailHeight = 0;
	m_nHeight = 0;
	
	m_MaxVal = 100;
	m_MinVal = 0;

	m_nHead = 0;
	m_nBody = 0;
	m_nTail = 0;
	
	m_TransColor = RGB(255, 0, 255);
}

CSkinSlide::~CSkinSlide()
{
	if ( m_bmpHead )	delete m_bmpHead;
	if ( m_bmpBody )	delete m_bmpBody;
	if ( m_bmpTail )	delete m_bmpTail;
}


BEGIN_MESSAGE_MAP(CSkinSlide, CButton)
	//{{AFX_MSG_MAP(CSkinSlide)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinSlide message handlers
void CSkinSlide::SetType( int nType )
{
	m_Type = nType;
}
void CSkinSlide::SetHeadBmp( char* HeadBmpFile, int nHeight )
{
	m_HeadHeight = nHeight;
	m_bmpHead= new CBitmap;
	LoadBitmapFile( *m_bmpHead, HeadBmpFile );
	GetNofBody();
}

void CSkinSlide::SetBodyBmp( char* BodyBmpFile, int nHeight )
{
	m_BodyHeight = nHeight;
	m_bmpBody = new CBitmap;
	LoadBitmapFile( *m_bmpBody, BodyBmpFile );
	GetNofBody();
}

void CSkinSlide::SetTailBmp( char* TailBmpFile, int nHeight )
{
	m_TailHeight = nHeight;
	m_bmpTail = new CBitmap;
	LoadBitmapFile( *m_bmpTail, TailBmpFile );
	GetNofBody();
}

void CSkinSlide::SetTransColor( UINT TransColor )
{
	m_TransColor = TransColor;
}

UINT CSkinSlide::GetTransColor()
{
	return m_TransColor;
}

void CSkinSlide::GetNofBody()
{
	if ( m_BodyHeight == 0 )	{
		m_BodyCnt = 0;	return;
	}

	m_BodyCnt = (int)( ( (double)m_nHeight - m_HeadHeight - m_TailHeight ) 
						/ m_BodyHeight + 0.5 );
}

void CSkinSlide::SetRange( int MinVal, int MaxVal )
{
	m_MinVal = MinVal;
	m_MaxVal = MaxVal;
	m_OldVal = MinVal - 1;
}

void CSkinSlide::SetValue( int Val )
{
	if ( Val < m_MinVal || Val > m_MaxVal )	return;

	m_CurVal = Val;
	SendValue();
	m_CurY = GetPosByVal( Val );

	DrawSlide();
}

int CSkinSlide::GetValue()
{
	return m_CurVal;
}

void CSkinSlide::SendValue()
{
	if ( m_CurVal == m_OldVal )	return;
	m_OldVal = m_CurVal;

	if ( m_Type == 0 )	GetParent()->SendMessage( SL_CHANGE_MIC_VAL, (WPARAM)this, m_CurVal );
	else	GetParent()->SendMessage( SL_CHANGE_SPK_VAL, (WPARAM)this, m_CurVal );
}

void CSkinSlide::SizeToContent(int x, int y, int nWidth, int nHeight, BOOL bRedraw)
{
	m_nXPos = x;
	m_nYPos = y;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	
	m_CurVal = 0;
	m_OldVal = -1;
	m_CurY = m_TargetY = nHeight - 1;

	GetNofBody();

	MoveWindow(x, y, nWidth, nHeight, bRedraw);
}

void CSkinSlide::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	int		i, y;
	CDC		MemDC;	
	CBitmap* pbmpOld;

	ASSERT(lpDrawItemStruct != NULL);
	UINT nState = lpDrawItemStruct->itemState;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if ( m_nTail == 0 )	goto ExitTag;

	MemDC.CreateCompatibleDC(pDC);	

	pbmpOld = MemDC.SelectObject( m_bmpTail );		
	y = m_nHeight - m_TailHeight;
	::TransparentBlt( pDC->m_hDC, 0, y, m_nWidth, m_TailHeight, 
				MemDC.m_hDC, 0, 0, m_nWidth, m_TailHeight, m_TransColor );
	MemDC.SelectObject( pbmpOld );

	pbmpOld = MemDC.SelectObject( m_bmpBody );
	for ( i = 0; i < m_nBody; i++ )	{
		y -= m_BodyHeight;
		::TransparentBlt( pDC->m_hDC, 0, y, m_nWidth, m_BodyHeight, 
				MemDC.m_hDC, 0, 0, m_nWidth, m_BodyHeight, m_TransColor );
	}
	MemDC.SelectObject( pbmpOld );

	if ( m_nHead == 0 )	goto ExitTag;

	pbmpOld = MemDC.SelectObject( m_bmpHead );
	y -= m_HeadHeight;
	::TransparentBlt( pDC->m_hDC, 0, y, m_nWidth, m_HeadHeight, 
			MemDC.m_hDC, 0, 0, m_nWidth, m_HeadHeight, m_TransColor );
	MemDC.SelectObject( pbmpOld );

ExitTag :
	ReleaseDC(pDC);
}

void CSkinSlide::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_TargetY = point.y;
	m_CurVal = GetValByPos( m_CurY );
	SendValue();

	DrawSlide();
	SetTimer(1100, 10, NULL);
	SetCapture();
	m_bMoving = TRUE;
}

void CSkinSlide::OnLButtonUp(UINT nFlags, CPoint point) 
{
	KillTimer(1100);
	::ReleaseCapture();
	m_bMoving = FALSE;
}

void CSkinSlide::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( (GetForegroundWindow() == GetParent() || 
		GetForegroundWindow() == GetParent()->GetOwner()) 
		&& m_bMoving )
	{
		if ( point.y < 0 )	
			m_TargetY = 0;
		else if ( point.y >= m_nHeight )	
			m_TargetY = m_nHeight - 1;
		else
			m_TargetY = point.y;
	}
	CButton::OnMouseMove(nFlags, point);
}

void CSkinSlide::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_TargetY = m_CurY = point.y;
	m_CurVal = GetValByPos( m_CurY );
	SendValue();

	DrawSlide();

	CButton::OnLButtonDblClk(nFlags, point);
}

void CSkinSlide::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == 1100 )
	{
		if ( m_CurY == m_TargetY )	return;

/*
		CPoint point;
		GetCursorPos(&point);
		
		if (CWnd::WindowFromPoint(point) != this)	{
			KillTimer( 1100 );
			::ReleaseCapture();
			m_bMoving = FALSE;
			return;
		}
*/
		if ( m_CurY < m_TargetY )	m_CurY++;
		else m_CurY--;

		m_CurVal = GetValByPos( m_CurY );
		SendValue();

		DrawSlide();
	}

	CButton::OnTimer(nIDEvent);
}

int	CSkinSlide::GetValByPos( int yPos )
{
	// y = ( x - x1 ) * ( y2 - y1 ) / ( x2 - x1 ) + y1 
	// x1 = 0;	x2 = m_nHeight - 1;	y1 = m_MaxVal;	y2 = m_MinVal;	x = yPos
	// y = ( yPos - 0 ) * ( m_MinVal - m_MaxVal ) / ( m_nHeight - 1 - 0 ) + m_MaxVal;

	return  (int)( yPos * (double)( m_MinVal - m_MaxVal ) 
				/ ( m_nHeight - 1 ) + m_MaxVal + 0.5 );
}

int	CSkinSlide::GetPosByVal( int Val )
{
	// y = ( x - x1 ) * ( y2 - y1 ) / ( x2 - x1 ) + y1 
	// x1 = m_MaxVal;	x2 = m_MinVal;	y1 = 0;	y2 = m_nHeight - 1;	x = Val
	// y = ( Val - m_MaxVal ) * ( m_nHeight - 1 - 0 ) / ( m_MinVal - m_MaxVal ) + 0;

	return (int)( ( Val - m_MaxVal ) * (double)( m_nHeight - 1 )
				/ ( m_MinVal - m_MaxVal ) + 0.5 );
}

void CSkinSlide::DrawSlide()
{
	int		nHead, nBody, nTail;

	GetUnitCount( &nHead, &nBody, &nTail );
	if ( m_nHead == nHead && m_nBody == nBody && m_nTail == nTail )	return;

	m_nHead = nHead;
	m_nBody = nBody;
	m_nTail = nTail;

	ShowWindow(SW_HIDE);
	ShowWindow(SW_SHOW);	
}

void CSkinSlide::GetUnitCount( int *nHead, int *nBody, int *nTail )
{
	int		y = m_nHeight - m_CurY - 1;

	*nHead = *nBody = *nTail = 0;
	if ( y )	{
		*nTail = 1;	y -= m_TailHeight;
	}
	if ( y <= 0 || !m_BodyHeight )	return;

	*nBody = y / m_BodyHeight;
	if ( y % m_BodyHeight )	(*nBody)++;

	if ( *nBody > m_BodyCnt )	{
		*nBody = m_BodyCnt;
		*nHead = 1;
	}
}