// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "BISConManager.h"

#include "KeyLockCtrlDll.h"

#include "terminal.h"

#include "MainFrm.h"
#include "resource.h"
#include "stateview.h"

int GetKeyLockMaxCall();

extern CBISConManagerApp theApp;

UINT TerminalSendInfo(PVOID pArg);



#define TID_VIEW_UPDATE		10101
#define TVAL_VIEW_UPDATE	1000

static UINT BASED_CODE toolStartIVR[] =
{
	ID_APP_IVR_START,
	ID_SEPARATOR,
	ID_APP_ABOUT
};

static UINT BASED_CODE toolStopIVR[] =
{
	ID_APP_IVR_STOP,
	ID_SEPARATOR,
	ID_APP_ABOUT
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_ACTIVATE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_APP_IVR_START, OnAppIvrStart)
	ON_COMMAND(ID_APP_IVR_STOP, OnAppIvrStop)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	this->m_bIVRRegistered = 0;
	// Only Addpated Single Document.
	theApp.m_pMainFrame = this;
	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	SetWindowText("IVR Control Viewer");
	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NOBORDERS, 200);
	m_wndStatusBar.SetPaneText(1, "IVR OFF");
	set_tb_ivr_start();
	csBarString = "Ready";
	return 0;	
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	// TODO: Add your message handler code here
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// 2004 04 30 04 20, by wonny
	if(theApp.bIvrStart)
	{
		m_wndStatusBar.SetPaneText(1, "IVR ON");
	}
	else
	{
		m_wndStatusBar.SetPaneText(1, "IVR OFF");
	}
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	theApp.IVRMSGProc(message, wParam, lParam);	
	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

void CMainFrame::OnClose() 
{
	if ( MessageBox("Close Ivr Controller ?", "Close confirm", MB_OKCANCEL) != IDOK ) {
		return;
	}	
	CFrameWnd::OnClose();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)

{
	if( nID == SC_MINIMIZE )
	{
		ShowWindow( SW_HIDE );
	}
	CWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnAppIvrStart() 
{
	CBISConManagerApp * pApp = (CBISConManagerApp * )AfxGetApp();
	if( pApp )
	{
		pApp->m_pIVR_System.Ivr_MaxCall = GetKeyLockMaxCall();
		//if( pApp->m_pStateView )
		//{
		//	pApp->m_pStateView->SetInitView();
		//}
	}
	theApp.OnVoiconOp();
	//set_tb_ivr_stop();	
}

void CMainFrame::OnAppIvrStop() 
{
	theApp.OnVoiconOp();
	//set_tb_ivr_start();
}

void CMainFrame::set_tb_ivr_start()
{
	this->m_wndToolBar.SetButtons(toolStartIVR, sizeof(toolStartIVR)/sizeof(UINT));
	this->m_wndToolBar.LoadBitmap(IDB_START_REG);
}

void CMainFrame::set_tb_ivr_stop()
{
	this->m_wndToolBar.SetButtons(toolStopIVR, sizeof(toolStopIVR)/sizeof(UINT));
	if( m_bIVRRegistered )
		this->m_wndToolBar.LoadBitmap(IDB_STOP_REG);
	else
		this->m_wndToolBar.LoadBitmap(IDB_STOP_UNREG);
	
}

BOOL CMainFrame::EnableMenuItem(UINT id,UINT s)
{
	UINT state;
	CMenu * cMenu = this->GetMenu();
	if( s == MF_DISABLED )
		state = s|MF_GRAYED;
	else
		state = s;
	cMenu->EnableMenuItem(id, state);
	return TRUE;
}

void CMainFrame::DisableMenuItem(UINT id)
{
	this->EnableMenuItem(id, MF_DISABLED);
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnMouseMove(nFlags, point);
}

//void CMainFrame::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	// TODO: Add your control notification handler code here
//	
//	*pResult = 0;
//}

//LRESULT CMainFrame::OnResetMsg(WPARAM wParam,LPARAM lParam)
//{
//	//theApp.m_pParsor
//	return NULL;
//}

//void CMainFrame::OnRButtonUp(UINT nFlags, CPoint point) 
//{
//	// TODO: Add your message handler code here and/or call default
//	
//	CFrameWnd::OnRButtonUp(nFlags, point);
//}

void CMainFrame::SetStatusBarPaneText(int iCount, char *szText)
{
	m_wndStatusBar.SetPaneText(iCount, szText);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	
	
	CFrameWnd::OnTimer(nIDEvent);
}
