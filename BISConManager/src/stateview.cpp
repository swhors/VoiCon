// StateView.cpp : implementation file
//

#include "stdafx.h"
#include "BISConManager.h"
#include "StateView.h"
#include "IVRParsor.h"

#include "msg_define.h"

#include "CallInfoDlg.h"

extern	CBISConManagerApp theApp;
extern  int g_nCallState[200];

extern CCallStateBuf * pStateBuf;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStateView

IMPLEMENT_DYNCREATE(CStateView, CFormView)

CStateView::CStateView()
	: CFormView(CStateView::IDD)
{
	//{{AFX_DATA_INIT(CStateView)
	//}}AFX_DATA_INIT
	m_bUpdate	=	0;
	theApp.m_pStateView = this;
	m_nActive = 1;
	m_nTotalCall = 0;
	m_iListSelectedItem = 0;
}

CStateView::~CStateView()
{
	theApp.m_pStateView	 = NULL;
	m_nActive = 0;
	bInited = FALSE;
}

void CStateView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStateView)
	DDX_Control(pDX, IDC_ED_CUR_CALL, m_edCurCount);
	DDX_Control(pDX, IDC_TOTALCALLTITLE, m_stTotalTitle);
	DDX_Control(pDX, IDC_ED_TOTAL, m_edTotalCount);
	DDX_Control(pDX, IDC_CURRENTCALLTITLE, m_cCurrentTitle);
	DDX_Control(pDX, IDC_LIST2, m_cCallListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStateView, CFormView)
	//{{AFX_MSG_MAP(CStateView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_GETINFOTIP, IDC_LIST2, OnInfoTip)
	ON_COMMAND(IDM_POP_CALLINFO, OnPopCallinfo)
	ON_COMMAND(IDM_POP_RESET, OnPopReset)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkList2)
	ON_WM_RBUTTONUP()
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, OnRclickList2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStateView diagnostics

#ifdef _DEBUG
void CStateView::AssertValid() const
{
	CFormView::AssertValid();
}

void CStateView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStateView message handlers

void CStateView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	//if( !bInited )
	//	bInited = TRUE;
	//else
	//	return;
	DWORD dwStyle = m_cCallListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_TRACKSELECT;
	dwStyle |= LVS_EX_INFOTIP;
	
	this->m_cCallListCtrl.SetExtendedStyle(dwStyle);	
	//this->m_cCallListCtrl.EnableTrackingToolTips();
	//this->m_cCallListCtrl.EnableToolTips();
	SetInitView();
}

void CStateView::SetInitView()
{
	int	nImage = 0;
#if 0
	m_cCallListCtrl.DeleteAllItems();
#endif
	m_cCallListCtrl.DeleteColumn(3);
	m_cCallListCtrl.DeleteColumn(2);
	m_cCallListCtrl.DeleteColumn(1);
	m_cCallListCtrl.DeleteColumn(0);
	m_cCallListCtrl.InsertColumn(0, "No", LVCFMT_LEFT, 40, -1);
	m_cCallListCtrl.InsertColumn(1, "State", LVCFMT_LEFT,	120, -1);
	m_cCallListCtrl.InsertColumn(2, "Calling", LVCFMT_LEFT, 150, -1);
	m_cCallListCtrl.InsertColumn(3, "Called", LVCFMT_LEFT, 150, -1);
	
	CBitmap bm;
	bm.LoadBitmap(IDB_TELSTATELARGE);
	if( !this->m_LargeImageList.m_hImageList )
	{
		m_LargeImageList.Create( 32, 32, ILC_MASK | ILC_COLOR24, 0, 5 );
	}
	m_LargeImageList.Add(&bm, RGB( 255,255, 255 ));
	m_cCallListCtrl.SetImageList(&m_LargeImageList, LVSIL_NORMAL);
	
	CString csbuf;

	for( int i = 0; i < theApp.m_pIVR_System.Ivr_MaxCall; i++)
	{
		csbuf.Format("Waitting\nStep:0");
		m_cCallListCtrl.InsertItem(i, csbuf, 0);
	}
	
	int	cur, tol;
	CString str;
	cur = 0;
	tol = 0;

	str.Format(" %09d", cur );
	m_edTotalCount.SetWindowText(str);
	str.Format(" %03d", tol );
	m_edCurCount.SetWindowText(str);

	m_stTotalTitle.GetClientRect(&ctrl_rect );
	m_edTotalCount.GetClientRect(&ctrl_rect2 );
	m_cCurrentTitle.GetClientRect(&ctrl_rect3);
	m_edCurCount.GetClientRect( &ctrl_rect4 );
	
	m_nTotalCall = 0;
	
	m_bUpdate = 1 ;
}

void CStateView::StateChange(int channel, int state, CString sE164, CString dE164, int sCenarioStep)
{
	CString csbuf;//, logbuff;
	int nImage = 0;
	// state 
	
	// 0 wait, 1 calling,2 connected,3 busying, 4 outbound call connect, 5 disconnect
	g_nCallState[channel] = state;
	switch( state )
	{
	case 1:
		csbuf.Format("Calling\nStep:%d", sCenarioStep);
		break;
	case 2:
		csbuf.Format("Connected\nStep:%d", sCenarioStep);
		break;
	case 3:
		csbuf.Format("Busying\nStep:%d", sCenarioStep);
		break;
	case 4:
		csbuf.Format("Connected\nStep:%d", sCenarioStep);
		break;
	case 5:
		csbuf.Format("Disconnect\nStep:%d", sCenarioStep);
		break;
	case 6:
		csbuf.Format("Disconnect\nStep:%d", sCenarioStep);
		break;
	default:
		pStateBuf[channel].sE164.Empty();
		pStateBuf[channel].dE164.Empty();
		
		csbuf.Format("Waitting\nStep:0");
		break;
	}
	
	CString text  = m_cCallListCtrl.GetItemText(channel, 0 );
	if( text.Compare(csbuf ) )
	{
		m_cCallListCtrl.SetItem(channel,
								0, 
								LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH,
								csbuf , g_nCallState[channel] , NULL, NULL, NULL );
	}
}

void CStateView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

int CStateView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CStateView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
//	int iTotalWidth;
	RECT full_rect;//, log_rect;
	if( m_bUpdate == 1 )
	{
		if(::IsWindow(this->m_hWnd))
		{
			int x1,x2,x3,x4,y2;
			int width1,width2,width3,width4,height;;
			
			GetClientRect( &full_rect );
			width1 = ctrl_rect.right-ctrl_rect.left;
			width2 = ctrl_rect2.right-ctrl_rect2.left;
			width3 = ctrl_rect3.right-ctrl_rect3.left;
			width4 = ctrl_rect4.right-ctrl_rect4.left;
			height = ctrl_rect.bottom-ctrl_rect.top;
			
			x1 = full_rect.left+10;
			x2 = x1+width1+4;
			x4 = full_rect.right-10-width4;
			x3 = x4-width3-4;
			y2 = full_rect.bottom-height-3;
			
			full_rect.bottom =  full_rect.bottom - 30 ;
			m_cCallListCtrl.MoveWindow( &full_rect , TRUE );
			m_cCallListCtrl.Arrange(0);
			m_stTotalTitle.MoveWindow(x1, y2, width1, height, TRUE );
			m_edTotalCount.MoveWindow(x2, y2, width2, height, TRUE );
			m_cCurrentTitle.MoveWindow(x3, y2, width3, height, TRUE );
			m_edCurCount.MoveWindow(x4, y2, width4, height, TRUE );
		}
	}
}

BOOL CStateView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CStateView::OnMouseMove(UINT nFlags, CPoint point)
{
	CFormView::OnMouseMove(nFlags, point);
}

BOOL CStateView::PreTranslateMessage(MSG* pMsg)
{
	return CFormView::PreTranslateMessage(pMsg);
}

void CStateView::OnInfoTip( NMHDR * pNMHDR, LRESULT * pResult )
{
	NMLVGETINFOTIP*	pInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
	ASSERT(pInfoTip);

	char tbuf[256];
	sprintf(tbuf, "%s\n -> %s", pStateBuf[pInfoTip->iItem].sE164, pStateBuf[pInfoTip->iItem].dE164);
	_tcsncpy(pInfoTip->pszText, (LPCTSTR)tbuf, pInfoTip->cchTextMax);
}

void CStateView::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LVITEM * pItem = (LVITEM *) pNMHDR;
	
	POSITION pos = this->m_cCallListCtrl.GetFirstSelectedItemPosition();
	this->m_iListSelectedItem=this->m_cCallListCtrl.GetNextSelectedItem(pos);
	if( this->m_iListSelectedItem >= 0 )
	{
		pop_menu(pos);
	}
	*pResult = 0;
}

void CStateView::pop_menu(POSITION &pos)
{
	HMENU hMenu;
	hMenu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MENU_POP_LIST));
	if( hMenu != NULL )
	{
		CPoint cPoint;
		HMENU hmenuTrackPopup = GetSubMenu(hMenu, 0);
		WINDOWPLACEMENT wPlace,wPlace1;
		GetActiveWindow()->GetWindowPlacement(&wPlace);
		GetWindowPlacement(&wPlace1);
		this->m_cCallListCtrl.GetItemPosition(this->m_iListSelectedItem, &cPoint);
		cPoint.x = cPoint.x + wPlace.rcNormalPosition.left
							+wPlace1.rcNormalPosition.left;
		cPoint.y = cPoint.y + wPlace.rcNormalPosition.top
							+ wPlace1.rcNormalPosition.top+50;
		TrackPopupMenu(hmenuTrackPopup,
			TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			cPoint.x, cPoint.y, 0, m_hWnd, NULL);
		DestroyMenu(hMenu); 
	}
}

void CStateView::init_count_state()
{	//this->m_cbListBox.SetColumnWidth(4);
}

void CStateView::OnPopCallinfo() 
{
	CCallInfoDlg	*pdlg;
	pdlg = new CCallInfoDlg(this->m_iListSelectedItem);;
	if( pdlg )
	{
		pdlg->Create(IDD_DLG_CINF,this);
		pdlg->ShowWindow(SW_SHOW);

	}
}

void CStateView::OnPopReset() 
{
	::SendMessage(HWND_BROADCAST, theApp.MessageReq, ACRO_IVR_RESETCALL, this->m_iListSelectedItem);
}

void CStateView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//	POSITION pos = this->m_cCallListCtrl.GetFirstSelectedItemPosition();
	//	this->m_iListSelectedItem=this->m_cCallListCtrl.GetNextSelectedItem(pos);
	//	pop_menu(pos);
	//	CFormView::OnRButtonUp(nFlags, point);
}

void CStateView::OnRclickList2(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
}

void CStateView::UpdateCurrentCall()
{
	int nCurrentCall	=	0;
	for( int count = 0 ; count < 200 ; count++)
	{
		if( (g_nCallState[count] == 2) || (g_nCallState[count] == 3) || (g_nCallState[count] == 4) )
			nCurrentCall ++;
	}
	CString str;
	str.Format(" %03d", nCurrentCall );
	m_edCurCount.SetWindowText(str);
}
