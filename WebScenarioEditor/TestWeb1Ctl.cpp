// TestWeb1Ctl.cpp : Implementation of the CTestWeb1Ctrl ActiveX Control class.

#include "stdafx.h"
#include "afxdlgs.h"
#include "TestWeb1.h"
#include "TestWeb1Ctl.h"
#include "TestWeb1Ppg.h"

#include "ScenarioEditorControlFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTestWeb1Ctrl, COleControl)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CTestWeb1Ctrl, COleControl)
	//{{AFX_MSG_MAP(CTestWeb1Ctrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CTestWeb1Ctrl, COleControl)
	//{{AFX_DISPATCH_MAP(CTestWeb1Ctrl)
	DISP_FUNCTION(CTestWeb1Ctrl, "ResetEditor", ResetEditor, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemCondition", ItemCondition, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemHub", ItemHub, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemAndOr", ItemAndOr, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemLoop", ItemLoop, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemMath", ItemMath, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemIS", ItemIS, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemRetrial", ItemRetrial, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemPoint", ItemPoint, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemScePage", ItemScePage, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemInit", ItemInit, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemPlay", ItemPlay, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemInput", ItemInput, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemPlug", ItemPlug, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemTime", ItemTime, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "SaveEditor", SaveEditor, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "LoadEditor", LoadEditor, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemPhone", ItemPhone, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTestWeb1Ctrl, "ItemVoiceRec", ItemVoiceRec, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CTestWeb1Ctrl, COleControl)
	//{{AFX_EVENT_MAP(CTestWeb1Ctrl)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CTestWeb1Ctrl, 1)
	PROPPAGEID(CTestWeb1PropPage::guid)
END_PROPPAGEIDS(CTestWeb1Ctrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CTestWeb1Ctrl, "TESTWEB1.TestWeb1Ctrl.1",
	0xe1017284, 0xc8b4, 0x4748, 0x92, 0x63, 0x68, 0xed, 0xe0, 0x27, 0x4a, 0x84)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CTestWeb1Ctrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DTestWeb1 =
		{ 0xbbae7119, 0x276a, 0x4671, { 0x8c, 0xac, 0x97, 0x8f, 0x29, 0x41, 0x7e, 0x90 } };
const IID BASED_CODE IID_DTestWeb1Events =
		{ 0xa56a06e3, 0xf6b8, 0x4515, { 0x95, 0x22, 0x23, 0x40, 0x40, 0xee, 0x70, 0xbf } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwTestWeb1OleMisc =
	OLEMISC_SIMPLEFRAME |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CTestWeb1Ctrl, IDS_TESTWEB1, _dwTestWeb1OleMisc)


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl::CTestWeb1CtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CTestWeb1Ctrl

BOOL CTestWeb1Ctrl::CTestWeb1CtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_TESTWEB1,
			IDB_TESTWEB1,
			afxRegApartmentThreading,
			_dwTestWeb1OleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl::CTestWeb1Ctrl - Constructor

CTestWeb1Ctrl::CTestWeb1Ctrl()
{
	InitializeIIDs(&IID_DTestWeb1, &IID_DTestWeb1Events);
	
	EnableSimpleFrame();
	ItemCount = -1;

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl::~CTestWeb1Ctrl - Destructor

CTestWeb1Ctrl::~CTestWeb1Ctrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl::OnDraw - Drawing function

void CTestWeb1Ctrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if( this->m_pTestFormView )
	{
		WINDOWPLACEMENT place;
		place.rcNormalPosition.top = 0;
		place.rcNormalPosition.left = 0;
		place.rcNormalPosition.right = 600;
		place.rcNormalPosition.bottom = 600;
		this->m_pTestFormView->SetWindowPlacement(&place);
		this->m_pTestFormView->m_editor.SetWindowPlacement(&place);
	}
	// TODO: Replace the following code with your own drawing code.
}

/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl::DoPropExchange - Persistence support

void CTestWeb1Ctrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
	// TODO: Call PX_ functions for each persistent custom property.
}

/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl::OnResetState - Reset control to default state

void CTestWeb1Ctrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl message handlers

int CTestWeb1Ctrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRuntimeClass* pViewClass = RUNTIME_CLASS(CWebFormView);
	m_pTestFormView = (CWebFormView*) pViewClass->CreateObject();
	if (m_pTestFormView == NULL)
	{
		TRACE1("Warning: Dynamic create of view type %Fs failed\n", pViewClass->m_lpszClassName);
		return -1;
	}
	CCreateContext context;
	context.m_pNewViewClass = pViewClass;
	context.m_pCurrentDoc = NULL;
	if (!m_pTestFormView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 600, 200), this, AFX_IDW_PANE_FIRST, &context))
	{
		TRACE0("Warning: couldn't create view for frame\n");
		delete m_pTestFormView;
		return -1;
	}
	m_pTestFormView->m_pDoc = this;
	m_pTestFormView->OnInitialUpdate();
  return 0;
}

void CTestWeb1Ctrl::ResetEditor()
{
	//this->m_pTestFormView->SetWindowText("Test");
	this->m_objs.RemoveAll();
	//this->m_objs.ClearLinks();
	this->cItem.DelItem();
	this->m_pTestFormView->RedrawWindow();
}

void CTestWeb1Ctrl::ItemHub()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_HUB;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_HUB, 0);
	}
}

void CTestWeb1Ctrl::ItemScePage()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_PAGE;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_PAGE, 0);
	}
}

void CTestWeb1Ctrl::ItemPoint()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_GOTO;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_GOTO, 0);
	}
}

void CTestWeb1Ctrl::ItemInit()
{
	if( this->m_pTestFormView )
	{
		if( !cItem.HaveTop() )
		{
			m_pTestFormView->m_iSelectedItem = IDT_INIT;
			this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_INIT, 0);
		}
	}
}

void CTestWeb1Ctrl::ItemCondition()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_CONDITION;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_EQUAL, 0);
	}
}

void CTestWeb1Ctrl::ItemAndOr()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_LOGICAL;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_ANDOR, 0);
	}
}

void CTestWeb1Ctrl::ItemLoop()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_LOOP;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_CIRCUIT, 0);
	}
}

void CTestWeb1Ctrl::ItemMath()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_MATH;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_MATH, 0);
	}
}

void CTestWeb1Ctrl::ItemIS()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_IS;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_IS, 0);
	}
}

void CTestWeb1Ctrl::ItemRetrial()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_RETRIAL;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_RETRIAL, 0);
	}
}

void CTestWeb1Ctrl::ItemPlay()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_PLAY;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_SPEAK, 0);
	}
}

void CTestWeb1Ctrl::ItemInput()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_INPUT;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_DTMF, 0);
	}
}

void CTestWeb1Ctrl::ItemPlug()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_PLUG;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_PLUG, 0);
	}
}

void CTestWeb1Ctrl::ItemTime()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_TIMECHK;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_WATCH, 0);
	}
}

void CTestWeb1Ctrl::ItemPhone() 
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_MAKE_CALL;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_PHONE, 0);
	}
}

void CTestWeb1Ctrl::LoadEditor()
{
	int bFind = 0;
	CStdioFile cStreamFile;
	CFileDialog pDlg(TRUE,"sce","*.sce",NULL,"sce");
	if( pDlg.DoModal() == IDOK )
	{
		CString csTitle = pDlg.GetPathName();
		if( cStreamFile.Open(csTitle,CFile::modeRead,NULL) )
		{
			m_objs.Clear();
			CString str;
			cStreamFile.ReadString(str);
			if( !str.Compare("1\r"))
				bFind = 1;
			if( !str.Compare("1"))
				bFind = 1;
			if( !str.Compare("1\r\n"))
				bFind = 1;
			if( !bFind )
				return;
			while(cStreamFile.ReadString( str ) )
			{
	
				if( !m_objs.FromString( str ) )
				{
					CDiagramEntity* obj = CScenarioEditorControlFactory::CreateFromString( str );
					if( obj )
					{
						obj->SetDocument(this);
						m_objs.Add( obj );
					}
					else
					{
						CItemLink* link = new CItemLink;
						if( link->FromString( str ) )
							m_objs.AddLink( link );
						else
							delete link;
					}
				}
			}
			m_objs.SetModified( FALSE );
			csTitle.Replace(".sce","");
			csTitle.Replace(".SCE","");
			this->ItemCount = this->cItem.ReadFromFile(csTitle);
			this->m_pTestFormView->OnInitialUpdate();
			this->m_pTestFormView->RedrawWindow();
		}
	}
}

//한양디지텍

void CTestWeb1Ctrl::SaveEditor()
{
	CStdioFile cStreamFile;
	CFileDialog pDlg(FALSE,"sce","*.sce",NULL,"sce");
	if( !this->cItem.HaveTop() )
	{
		AfxMessageBox("Not have TOP Item:[init]");
		return;
	}
	if( pDlg.DoModal() == IDOK )
	{
		CString csFile = pDlg.GetPathName();
		if( cStreamFile.Open(csFile,CFile::modeWrite|CFile::modeCreate ,NULL) )
		{
			cStreamFile.WriteString("1\r\n");
			cStreamFile.WriteString( m_objs.GetString() + _T( "\r\n" ) );
			int count = 0;
			CDiagramEntity* obj;
			while( ( obj = m_objs.GetAt( count++ ) ) )
				cStreamFile.WriteString( obj->GetString() + _T( "\r\n" ) );
			
			int max = m_objs.GetLinks();
			for( int t = 0 ; t < max ; t++ )
			{
				CItemLink* link = m_objs.GetLinkAt( t );
				if( link )
					cStreamFile.WriteString( link->GetString() + _T( "\r\n" ) );
			}
			m_objs.SetModified( FALSE );
			
			csFile.Replace(".sce","");
			csFile.Replace(".SCE","");
			this->cItem.WriteToFile(csFile);
		}
	}
}

void CTestWeb1Ctrl::ItemVoiceRec()
{
	if( this->m_pTestFormView )
	{
		this->m_pTestFormView->m_iSelectedItem = IDT_VOICE_REC;
		this->m_pTestFormView->SendMessage(WM_VIEW_ADD,IDI_VOICE_REC, 0);
	}
}
