// ViewPage2.cpp : implementation file
//

#include "stdafx.h"
#include "ScenarioEditor.h"
#include "ViewPage2.h"
#include "ScenarioEditorDoc.h"

#include "ItemLinkableLineSegment.h"

#include "typedef.h"

extern CScenarioEditorApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewPage2

IMPLEMENT_DYNCREATE(CViewPage2, CFormView)

CViewPage2::CViewPage2()
	: CFormView(CViewPage2::IDD)
{
	//{{AFX_DATA_INIT(CViewPage2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_screenResolutionX = 0;
	this->m_preHorzSize = 0;
	this->m_preVertSize = 0;
}

CViewPage2::~CViewPage2()
{
}

void CViewPage2::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewPage2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewPage2, CFormView)
	//{{AFX_MSG_MAP(CViewPage2)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOVE()
	ON_MESSAGE(WM_VIEW_ADD, OnUpdateItem)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewPage2 diagnostics

#ifdef _DEBUG
void CViewPage2::AssertValid() const
{
	CFormView::AssertValid();
}

void CViewPage2::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewPage2 message handlers

void CViewPage2::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CFormView::OnLButtonDown(nFlags, point);
}

void CViewPage2::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CFormView::OnLButtonUp(nFlags, point);
}

void CViewPage2::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnRButtonDown(nFlags, point);
}

void CViewPage2::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnRButtonUp(nFlags, point);
}

void CViewPage2::OnMouseMove(UINT nFlags, CPoint point) 
{
	CFormView::OnMouseMove(nFlags, point);
}

void CViewPage2::OnMove(int x, int y) 
{
	CFormView::OnMove(x, y);
	
	// TODO: Add your message handler code here
	
}

void CViewPage2::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage = this;
	CRect rect;
	GetClientRect( rect );
	if( !m_editor.m_hWnd )
	{
		// Creating the editor window
		CScenarioEditorDoc * pDoc = (CScenarioEditorDoc *)GetDocument();
		pDoc->GetData()->SetClipboardHandler( &theApp.m_clip );
		m_editor.Create( WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL, rect, this, pDoc->GetData() );
		m_editor.SetDocument(this->GetDocument());
		this->m_preVertSize = rect.Height();
		this->m_preHorzSize = rect.Width();
	}
	else
	{
		RECT lRect;
		m_editor.Clear();
		m_editor.SetDocument(this->GetDocument());
		m_editor.GetClientRect(&lRect);
		rect = lRect;
		//GetClientRect( rect );
		CSize cSize = m_editor.GetViewSize();
		if( rect.Height() > cSize.cy )
			this->m_preVertSize = rect.Height();
		else
			this->m_preVertSize = cSize.cy+20;
		if( rect.Width() > cSize.cx )
			this->m_preHorzSize = rect.Width();
		else
			this->m_preHorzSize = cSize.cx+20;
	}
	CClientDC dc( this );
	m_screenResolutionX = dc.GetDeviceCaps( LOGPIXELSX );
	m_editor.SetVirtualSize( CSize( m_preHorzSize,m_preVertSize) );
	if( !m_editor.m_hWnd )
		m_editor.SetModified( FALSE );
}

void CViewPage2::OnDraw(CDC* pDC) 
{
	// TODO: Add your specialized code here and/or call the base class
	CScenarioEditorDoc* pDoc = (CScenarioEditorDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	if( pDC->IsPrinting() )
	{
		
		COLORREF col = m_editor.GetBackgroundColor();
		// Print zoom is the difference between screen- 
		// and printer resolution.
		double zoom = pDC->GetDeviceCaps( LOGPIXELSX ) / m_screenResolutionX;
		//zoom = 4;
		CRect rect( 0,0,
			( int )( ( double ) m_editor.GetVirtualSize().cx * zoom ),
			( int )( ( double ) m_editor.GetVirtualSize().cy * zoom ) );
		m_editor.SetRedraw( FALSE );
		m_editor.SetBackgroundColor( RGB( 255, 255, 255 ) );
		m_editor.Print( pDC, rect, zoom );
		m_editor.SetBackgroundColor( col );
		m_editor.SetRedraw( TRUE );
	}
}

CScenarioEditorDoc * CViewPage2::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CScenarioEditorDoc)));
	return (CScenarioEditorDoc*)m_pDocument;
}

LRESULT CViewPage2::OnUpdateItem(WPARAM wParam, LPARAM lParam)
{
	if( wParam != IDI_ARROW )
	{
		CScenarioEntity *obj = new CScenarioEntity( );
		obj->SetSymbol(wParam);
		obj->SetDocument(GetDocument());
		m_editor.StartDrawingObject( obj );
	}
	return NULL;
}

void CViewPage2::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if( m_editor.m_hWnd )
	{
		m_editor.MoveWindow(0,0,cx,cy);
		CRect rect;
		GetClientRect( rect );
		CClientDC dc( this );
		m_screenResolutionX = dc.GetDeviceCaps( LOGPIXELSX );
		if( rect.Height() > this->m_preVertSize )
			this->m_preVertSize = rect.Height();
		if( rect.Width() > this->m_preHorzSize )
			this->m_preHorzSize = rect.Width();
		m_editor.SetVirtualSize( CSize( m_preHorzSize,m_preVertSize) );
		m_editor.SetModified( FALSE );
	}
}
