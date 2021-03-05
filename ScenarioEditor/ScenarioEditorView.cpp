// ScenarioEditorView.cpp : implementation of the CScenarioEditorView class
//

#include "stdafx.h"
#include "ScenarioEditor.h"

#include "ScenarioEditorDoc.h"
#include "ScenarioEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorView

IMPLEMENT_DYNCREATE(CScenarioEditorView, CView)

BEGIN_MESSAGE_MAP(CScenarioEditorView, CView)
	//{{AFX_MSG_MAP(CScenarioEditorView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorView construction/destruction

CScenarioEditorView::CScenarioEditorView()
{
	// TODO: add construction code here

}

CScenarioEditorView::~CScenarioEditorView()
{
}

BOOL CScenarioEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorView drawing

void CScenarioEditorView::OnDraw(CDC* pDC)
{
	CScenarioEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorView printing

BOOL CScenarioEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CScenarioEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CScenarioEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorView diagnostics

#ifdef _DEBUG
void CScenarioEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CScenarioEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CScenarioEditorDoc* CScenarioEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CScenarioEditorDoc)));
	return (CScenarioEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorView message handlers

void CScenarioEditorView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	
}

void CScenarioEditorView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CView::CalcWindowRect(lpClientRect, nAdjustType);
}

void CScenarioEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnLButtonDown(nFlags, point);
}

void CScenarioEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnLButtonUp(nFlags, point);
}

void CScenarioEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnMouseMove(nFlags, point);
}
