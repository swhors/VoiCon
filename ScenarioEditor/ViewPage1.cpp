// ViewPage1.cpp : implementation file
//

#include "stdafx.h"
#include "ScenarioEditor.h"
#include "ScenarioEditorDoc.h"
#include "ViewPage1.h"
#include "typedef.h"

#include "item.h"

extern CItem	gItem;
extern CItem	ShowItem;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewPage1

IMPLEMENT_DYNCREATE(CViewPage1, CFormView)

CViewPage1::CViewPage1()
	: CFormView(CViewPage1::IDD)
{
	//{{AFX_DATA_INIT(CViewPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CViewPage1::~CViewPage1()
{
}

void CViewPage1::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewPage1)
	DDX_Control(pDX, IDC_BTN_CALLERR, m_btnCallErr);
	DDX_Control(pDX, IDC_ST_TYPE, m_sttype);
	DDX_Control(pDX, IDC_ST_GO, m_stgo);
	DDX_Control(pDX, IDC_ST_FALSEGO, m_stfalsego);
	DDX_Control(pDX, IDC_ST_CURID, m_stid);
	DDX_Control(pDX, IDC_BTN_PAGE, m_btnPage);
	DDX_Control(pDX, IDC_BTN_HUB, m_btnHub);
	DDX_Control(pDX, IDC_BTN_GOTO, m_btnGoto);
	DDX_Control(pDX, IDC_BTN_WATCH, m_btnWatch);
	DDX_Control(pDX, IDC_BTN_SPEAK, m_btnSpeak);
	DDX_Control(pDX, IDC_BTN_RETRIAL, m_btnRetrial);
	DDX_Control(pDX, IDC_BTN_PLUG, m_btnPlug);
	DDX_Control(pDX, IDC_BTN_PHONE, m_btnPhone);
	DDX_Control(pDX, IDC_BTN_MATH, m_btnMath);
	DDX_Control(pDX, IDC_BTN_IS, m_btnIs);
	DDX_Control(pDX, IDC_BTN_INIT, m_btnInit);
	DDX_Control(pDX, IDC_BTN_EQUAL, m_btnEqual);
	DDX_Control(pDX, IDC_BTN_DTMF, m_btnDtmf);
	DDX_Control(pDX, IDC_BTN_CIRCUIT, m_btnCircuit);
	DDX_Control(pDX, IDC_BTN_VOICE_REC, m_btnArrow);
	DDX_Control(pDX, IDC_BTN_ANDOR, m_btnAndor);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BTN_HUB, m_btnHub);
}


BEGIN_MESSAGE_MAP(CViewPage1, CFormView)
	//{{AFX_MSG_MAP(CViewPage1)
	ON_BN_CLICKED(IDC_BTN_INIT, OnBtnInit)
	ON_BN_CLICKED(IDC_BTN_ANDOR, OnBtnAndor)
	ON_BN_CLICKED(IDC_BTN_VOICE_REC, OnBtnArrow)
	ON_BN_CLICKED(IDC_BTN_CIRCUIT, OnBtnCircuit)
	ON_BN_CLICKED(IDC_BTN_DTMF, OnBtnDtmf)
	ON_BN_CLICKED(IDC_BTN_EQUAL, OnBtnEqual)
	ON_BN_CLICKED(IDC_BTN_IS, OnBtnIs)
	ON_BN_CLICKED(IDC_BTN_MATH, OnBtnMath)
	ON_BN_CLICKED(IDC_BTN_PHONE, OnBtnPhone)
	ON_BN_CLICKED(IDC_BTN_PLUG, OnBtnPlug)
	ON_BN_CLICKED(IDC_BTN_RETRIAL, OnBtnRetrial)
	ON_BN_CLICKED(IDC_BTN_SPEAK, OnBtnSpeak)
	ON_BN_CLICKED(IDC_BTN_WATCH, OnBtnWatch)
	ON_MESSAGE(WM_UPDATECURINFO,OnUpdateClientInfo)
	ON_BN_CLICKED(IDC_BTN_GOTO, OnBtnGoto)
	ON_BN_CLICKED(IDC_BTN_PAGE, OnBtnPage)
	ON_BN_CLICKED(IDC_BTN_CALLERR, OnBtnCallerr)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_HUB, OnBnClickedBtnHub)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewPage1 diagnostics

#ifdef _DEBUG
void CViewPage1::AssertValid() const
{
	CFormView::AssertValid();
}

void CViewPage1::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewPage1 message handlers

void CViewPage1::OnBtnInit() 
{
	// TODO: Add your control notification handler code here
	if( !((CScenarioEditorDoc*)GetDocument())->cItem.HaveTop() )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_INIT;
		if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
		{
			((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_INIT,0);
		}
	}
	//	SendMessage(WM_VIEW_ADD, IDI_INIT,0);
	//CNetworkSymbol*	obj = new CNetworkSymbol( IDB_BITMAP_SYMBOL_CLIENT );
	//m_editor.StartDrawingObject( obj );
}

void CViewPage1::OnBtnAndor() 
{
	// TODO: Add your control notification handler code here
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_LOGICAL;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_ANDOR,0);
	}
}

void CViewPage1::OnBtnArrow() 
{
	// TODO: Add your control notification handler code here
	//((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_ARROW;
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_VOICE_REC;
	//SendMessage(WM_DRAW_LINE);
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_VOICE_REC,0);
	}
}

void CViewPage1::OnBtnCircuit() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_LOOP;
		if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_CIRCUIT,0);
	}
}

void CViewPage1::OnBtnDtmf() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_INPUT;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_DTMF,0);
	}
}

void CViewPage1::OnBtnEqual() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_CONDITION;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_EQUAL,0);
	}
}

void CViewPage1::OnBtnIs() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_IS;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_IS,0);
	}
}

void CViewPage1::OnBtnMath() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_MATH;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_MATH,0);
	}
}

void CViewPage1::OnBtnPhone() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_MAKE_CALL;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_PHONE,0);
	}
}

void CViewPage1::OnBtnPlug() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_PLUG;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_PLUG,0);
	}
}

void CViewPage1::OnBtnRetrial() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_RETRIAL;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_RETRIAL,0);
	}
}

void CViewPage1::OnBtnSpeak() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_PLAY;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_SPEAK,0);
	}
}

void CViewPage1::OnBtnWatch() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_TIMECHK;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_WATCH,0);
	}
}

void CViewPage1::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	((CScenarioEditorApp*)AfxGetApp())->m_IndexPage = this;
	m_btnWatch.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_WATCH)));
	m_btnSpeak.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_SPEAK)));
	m_btnRetrial.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_RETRIAL)));
	m_btnPlug.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PLUG)));
	m_btnPhone.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PHONE)));
	m_btnMath.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_MATH)));
	m_btnIs.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_IS)));
	m_btnInit.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_INIT)));
	m_btnEqual.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_EQUAL)));
	m_btnDtmf.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_DTMF)));
	m_btnCircuit.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_CIRCUIT)));
	//m_btnArrow.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ARROW)));
	m_btnArrow.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_VOICE_REC)));
	m_btnAndor.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ANDOR)));
	m_btnGoto.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_GOTO)));
	m_btnHub.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_HUB)));
	m_btnPage.SetIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PAGE)));
	m_btnCallErr.SetIcon(LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CALLERR)));
	
	this->m_btnInit.SetToolTipText(new CString("Start of Scenario."));
	this->m_btnPhone.SetToolTipText(new CString("Call to Out-Bound."));
	this->m_btnEqual.SetToolTipText(new CString("A==B, A>B, A<B"));
	this->m_btnDtmf.SetToolTipText(new CString("Get DTMF Input."));
	this->m_btnAndor.SetToolTipText(new CString("A&&B or A||B"));
	this->m_btnCircuit.SetToolTipText(new CString("Sleep"));
	this->m_btnMath.SetToolTipText(new CString("A+b=c or A-B=c or A*B=C or A/B=C"));
	this->m_btnSpeak.SetToolTipText(new CString("Ment File"));
	this->m_btnPlug.SetToolTipText(new CString("Plug-In DLL"));
	this->m_btnWatch.SetToolTipText(new CString("Time Checking"));
	this->m_btnIs.SetToolTipText(new CString(""));
	this->m_btnGoto.SetToolTipText(new CString("Go to next point"));
	this->m_btnRetrial.SetToolTipText(new CString("Control retrial condition"));
	this->m_btnHub.SetToolTipText(new CString("Multipoint goto."));
	this->m_btnPage.SetToolTipText(new CString("Go to other scenario page."));
	this->m_btnCallErr.SetToolTipText(new CString("Action in case of call error."));
}

LRESULT	CViewPage1::OnUpdateClientInfo(WPARAM wParam,LPARAM lParam)
{
	char tbuf[64];
	if( ShowItem.GetCount() >= 0 )
	{
		wsprintf(tbuf, "%d", ShowItem.GetType());
		this->m_sttype.SetWindowText(tbuf);
		wsprintf(tbuf, "%d", ShowItem.GetCount());
		this->m_stid.SetWindowText(tbuf);
		wsprintf(tbuf, "%d", ShowItem.GetFalseGo());
		this->m_stfalsego.SetWindowText(tbuf);
		wsprintf(tbuf, "%d", ShowItem.GetGo());
		this->m_stgo.SetWindowText(tbuf);

	}
	return NULL;
}

void CViewPage1::OnBtnGoto() 
{
	// TODO: Add your control notification handler code here
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_GOTO;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_GOTO,0);
	}
}

void CViewPage1::OnBnClickedBtnHub()
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_HUB;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_HUB,0);
	}
}

void CViewPage1::OnBtnPage() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_PAGE;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_PAGE,0);
	}	
}

void CViewPage1::OnBtnCallerr() 
{
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_CALLERR;
	if(((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage )
	{
		((CScenarioEditorApp*)AfxGetApp())->m_MainViewPage->SendMessage(WM_VIEW_ADD,IDI_CALLERR,0);
	}	
}
