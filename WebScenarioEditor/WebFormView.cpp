// WebFormView.cpp : implementation file
//

#include "stdafx.h"
#include "TestWeb1.h"
#include "WebFormView.h"

#include "TestWeb1Ctl.h"

#include "TestWeb1.h"

extern CTestWeb1App NEAR theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebFormView

IMPLEMENT_DYNCREATE(CWebFormView, CFormView)

CWebFormView::CWebFormView()
	: CFormView(CWebFormView::IDD)
{
	//{{AFX_DATA_INIT(CWebFormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWebFormView::~CWebFormView()
{
}

void CWebFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebFormView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebFormView, CFormView)
	//{{AFX_MSG_MAP(CWebFormView)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_VIEW_ADD, OnUpdateItem)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebFormView diagnostics

#ifdef _DEBUG
void CWebFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWebFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWebFormView message handlers

BOOL CWebFormView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CWebFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	CRect rect;
	GetClientRect( rect );
	if( !m_editor.m_hWnd )
	{
		// Creating the editor window
		//rect = rect + rect.;
		rect.right = rect.right - 300;
		rect.bottom = rect.bottom - 150;
		this->m_preVertSize = rect.Height();//+rect.Height()/3;
		this->m_preHorzSize = rect.Width();//+rect.Width()/3;
		if( !this->m_pDoc )
			m_editor.Create( WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL, rect, this, NULL );
		else
			m_editor.Create( WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL, rect, this, m_pDoc->GetData() );
		m_editor.SetDocument(this->m_pDoc);
	}
	else
	{
		//m_editor.Clear();
		RECT lRect;
		m_editor.GetClientRect(&lRect);
		rect = lRect;
		CSize cSize = m_editor.GetViewSize();
		if( rect.Height() > cSize.cy )
			this->m_preVertSize = rect.Height();
		else
			this->m_preVertSize = cSize.cy+50;
		if( rect.Width() > cSize.cx )
			this->m_preHorzSize = rect.Width();
		else
			this->m_preHorzSize = cSize.cx+30;
	}

	CClientDC dc( this );
	m_screenResolutionX = dc.GetDeviceCaps( LOGPIXELSX );
	m_editor.SetVirtualSize( CSize( m_preHorzSize,m_preVertSize) );
	if( !m_editor.m_hWnd )
		m_editor.SetModified( FALSE );
}

void CWebFormView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CFormView::OnLButtonDown(nFlags, point);
}

void CWebFormView::OnDraw(CDC* pDC) 
{
	if( pDC->IsPrinting() )
	{

		COLORREF col = m_editor.GetBackgroundColor();
		// Print zoom is the difference between screen- 
		// and printer resolution.
		double zoom = pDC->GetDeviceCaps( LOGPIXELSX ) / m_screenResolutionX;
		
		//CRsct rect = 

		//CRect rect( 0,0, 
		//	( int )( ( double ) m_editor.GetVirtualSize().cx * zoom ), 
		//	( int )( ( double ) m_editor.GetVirtualSize().cy * zoom ) );
		m_editor.SetRedraw( FALSE );
		m_editor.SetBackgroundColor( RGB( 255, 255, 255 ) );
		//m_editor.Print( pDC, rect, zoom );
		m_editor.SetBackgroundColor( col );
		m_editor.SetRedraw( TRUE );
	}	
}

LRESULT CWebFormView::OnUpdateItem(WPARAM wParam, LPARAM lParam)
{
	if( wParam != IDI_ARROW )
	{
		CScenarioEntity *obj = new CScenarioEntity( );
		obj->SetSymbol(wParam);
		obj->SetDocument(this->m_pDoc);
		m_editor.StartDrawingObject(obj);
	}
	return NULL;
}

LRESULT CWebFormView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CFormView::DefWindowProc(message, wParam, lParam);
}

void CWebFormView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CFormView::OnRButtonDown(nFlags, point);
}

void CWebFormView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CFormView::OnLButtonUp(nFlags, point);
}

void CWebFormView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CFormView::OnRButtonUp(nFlags, point);
}

void CWebFormView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CFormView::OnRButtonDblClk(nFlags, point);
}
