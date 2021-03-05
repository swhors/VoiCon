// SkinLabel.cpp : implementation file
//

#include "stdafx.h"
#include "SkinLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinLabel

CSkinLabel::CSkinLabel()
{
	m_Str = "";
	m_SkinFont = NULL;
	m_TextFont = NULL;
	m_Align = TA_CENTER;
	m_MaxLength = -1;
}

CSkinLabel::~CSkinLabel()
{	
}


BEGIN_MESSAGE_MAP(CSkinLabel, CButton)
	//{{AFX_MSG_MAP(CSkinLabel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinLabel message handlers

void CSkinLabel::SetSkinFont( CSkinFont *SkinFont )
{
	m_SkinFont = SkinFont;
}

void CSkinLabel::SetSkinFont( t_Font *SkinFont )
{
	m_TextFont = SkinFont;
}

void CSkinLabel::SetTextAlign( UINT Align )
{
	m_Align = Align;
}

void CSkinLabel::SizeToContent(int x, int y, int nWidth, int nHeight, BOOL bRedraw)
{
	m_nXPos = x;
	m_nYPos = y;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	
	MoveWindow(x, y, nWidth, nHeight, bRedraw);
}

void CSkinLabel::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item	
	ShowWindow(SW_HIDE);
	Invalidate(FALSE);
	ShowWindow(SW_SHOW);
}

void CSkinLabel::TextOut( CDC *pDC )
{
	int		l, i, x, w, ww, h;
	CDC		MemDC;
	CBitmap* pbmpCh;
	CBitmap* pbmpOld;

	MemDC.CreateCompatibleDC(pDC); 

	l = strlen( m_Str );

	for ( i = ww = 0; i < l; i++ )	{
		if ( !m_SkinFont->GetFontCh( (char)m_Str[i], &w, &h, &pbmpCh ) )	continue;
		ww += ( w + m_SkinFont->GetChGapPixel() );
	}

	if		( m_Align == TA_LEFT )		x = 0;
	else if ( m_Align == TA_CENTER )	x = ( m_nWidth - ww ) / 2;
	else if ( m_Align == TA_RIGHT )		x = m_nWidth - ww;
	else	x = 0;;

	for ( i = 0; i < l; i++ )	{
		if ( !m_SkinFont->GetFontCh( (char)m_Str[i], &w, &h, &pbmpCh ) )	
			continue;

		pbmpOld = MemDC.SelectObject( pbmpCh );		
		::TransparentBlt( pDC->m_hDC, x, 0, w, h, 
					MemDC.m_hDC, 0, 0, w, h,
					m_SkinFont->GetTransColor() );
		MemDC.SelectObject( pbmpOld );

		x += ( w + m_SkinFont->GetChGapPixel() );
	}
}

void CSkinLabel::AddInt(int i)
{
}

void CSkinLabel::AddString( CString Str )
{
	m_Str += Str;

	if ( m_MaxLength > 0 && m_Str.GetLength() > m_MaxLength )
		m_Str = m_Str.Right( m_MaxLength );
	
	ShowWindow(SW_HIDE);
	CWnd::Invalidate( TRUE );
	ShowWindow(SW_SHOW);	
}

void CSkinLabel::SetLabel( CString Str )
{
	m_Str = Str;

	if ( m_MaxLength > 0 && m_Str.GetLength() > m_MaxLength )
		m_Str = m_Str.Right( m_MaxLength );
	
	ShowWindow(SW_HIDE);
	CWnd::Invalidate( TRUE );
	ShowWindow(SW_SHOW);	
}

CString CSkinLabel::GetLabel()
{
	return m_Str;
}

void CSkinLabel::DelChar()
{
	int		len;
	char	tmp[30] = { 0 };

	len = m_Str.GetLength();
	if ( len > 0 )	{
		strncpy( tmp, (char*)(LPCTSTR)m_Str, len - 1 );
		m_Str = tmp;
		ShowWindow(SW_HIDE);
		CWnd::Invalidate( TRUE );
		ShowWindow(SW_SHOW);	
	}	
}

void CSkinLabel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CFont font;
	LOGFONT LogFont;	

	if( m_SkinFont )
		TextOut( &dc );
	else if ( m_TextFont )
	{
		LogFont.lfHeight = -MulDiv( m_TextFont->size, GetDeviceCaps( dc, LOGPIXELSY), 72);
		LogFont.lfWidth = 0;
		LogFont.lfEscapement = 0;
		LogFont.lfOrientation = 0;
		LogFont.lfWeight = 0;
		LogFont.lfItalic = false;
		LogFont.lfUnderline = 0;
		LogFont.lfStrikeOut = 0;
		LogFont.lfCharSet = ANSI_CHARSET;
		LogFont.lfOutPrecision = OUT_TT_PRECIS;
		LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		LogFont.lfQuality = DEFAULT_QUALITY;
		LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		lstrcpy( LogFont.lfFaceName, m_TextFont->FontName );
		VERIFY( font.CreateFontIndirect( &LogFont ) );  // Create the font.
		CFont *oldF = dc.SelectObject(&font);
		dc.SetTextAlign( m_Align );
		GetWindowText(m_Str);
		dc.TextOut( m_nWidth/2, 0, m_Str );
		dc.SelectObject(oldF);
		
		
	}
	//ReleaseDC(pDC);	
	// Do not call CButton::OnPaint() for painting messages
}

void CSkinLabel::SetMaxLength( int len )
{
	m_MaxLength = len;
}