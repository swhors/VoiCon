// SkinTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ubidialer.h"
#include "SkinTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinTreeCtrl

CSkinTreeCtrl::CSkinTreeCtrl()
{
}

CSkinTreeCtrl::~CSkinTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CSkinTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CSkinTreeCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinTreeCtrl message handlers

void CSkinTreeCtrl::Init( int x, int y, int w, int h, COLORREF bkcolor )
{
	MoveWindow( x, y, w, h, TRUE );
	SetBkColor( bkcolor );
	m_BkColor = bkcolor;
}

void CSkinTreeCtrl::InitImageList( int w, int h )
{
	m_ImgList.Create( w, h, ILC_COLORDDB, 1, 1 );
	SetImageList( &m_ImgList, TVSIL_NORMAL );
}

BOOL	CSkinTreeCtrl::AddImage( char *bmpFile )
{
	CBitmap	bitMap;

	if ( !LoadBitmapFile( bitMap, bmpFile ) )	return FALSE;
	m_ImgList.Add( &bitMap, RGB(1, 1, 1) );

	return TRUE;
}

void CSkinTreeCtrl::SetItemFont(HTREEITEM hItem, LOGFONT& logfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.color = (COLORREF)-1;
	cf.logfont = logfont;
	m_mapColorFont[hItem] = cf;
}

void CSkinTreeCtrl::SetItemBold(HTREEITEM hItem, BOOL bBold)
{
	SetItemState( hItem, bBold ? TVIS_BOLD: 0, TVIS_BOLD );
}

void CSkinTreeCtrl::SetItemColor(HTREEITEM hItem, COLORREF color)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.logfont.lfFaceName[0] = '\0';
	cf.color = color;
	m_mapColorFont[hItem] = cf;
}

BOOL CSkinTreeCtrl::GetItemFont(HTREEITEM hItem, LOGFONT * plogfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return FALSE;
	if( cf.logfont.lfFaceName[0] == '\0' ) 
		return FALSE;
	*plogfont = cf.logfont;
	return TRUE;

}

BOOL CSkinTreeCtrl::GetItemBold(HTREEITEM hItem)
{
	return GetItemState( hItem, TVIS_BOLD ) & TVIS_BOLD;
}

COLORREF CSkinTreeCtrl::GetItemColor(HTREEITEM hItem)
{
	// Returns (COLORREF)-1 if color was not set
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return (COLORREF)-1;
	return cf.color;
}

void CSkinTreeCtrl::OnPaint() 
{
	CPaintDC dc(this);

	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );

	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();

	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	HTREEITEM hItem = GetFirstVisibleItem();
	
	int n = GetVisibleCount()+1;
	while( hItem && n--)
	{
		CRect rect;
		// Do not meddle with selected items or drop highlighted items
		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
		Color_Font cf;
		
		if ( !(GetItemState( hItem, selflag ) & selflag )
			&& m_mapColorFont.Lookup( hItem, cf ))
		{
			CFont *pFontDC;
			CFont fontDC;
			LOGFONT logfont;

			if( cf.logfont.lfFaceName[0] != '\0' )
			{
				logfont = cf.logfont;
			}
			else
			{
				// No font specified, so use window font
				CFont *pFont = GetFont();
				pFont->GetLogFont( &logfont );
			}
			if( GetItemBold( hItem ) )
				logfont.lfWeight = 700;
			fontDC.CreateFontIndirect( &logfont );
			pFontDC = memDC.SelectObject( &fontDC );
			
			if( cf.color != (COLORREF)-1 )
				memDC.SetTextColor( cf.color );

			CString sItem = GetItemText( hItem );

			GetItemRect( hItem, &rect, TRUE );
			memDC.SetBkColor( m_BkColor );
			memDC.TextOut( rect.left+2, rect.top+1, sItem );

			memDC.SelectObject( pFontDC );
		}
		hItem = GetNextVisibleItem( hItem );
	}


	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC,
				rcClip.left, rcClip.top, SRCCOPY );

}
