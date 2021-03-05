// ScenarioEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ScenarioEditor.h"

#include "MainFrm.h"
#include "ScenarioEditorDoc.h"
#include "ScenarioEditorView.h"

#include "typedef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorApp

BEGIN_MESSAGE_MAP(CScenarioEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CScenarioEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorApp construction

CScenarioEditorApp::CScenarioEditorApp()
{
	this->m_MainViewPage = NULL;
	this->m_iSelectedItem = IDT_NULL;
	this->m_IndexPage = NULL;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CScenarioEditorApp object

CScenarioEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorApp initialization

BOOL CScenarioEditorApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	
	AfxEnableControlContainer();
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CScenarioEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CScenarioEditorView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CScenarioEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorApp message handlers

DWORD CScenarioEditorApp::GetResourceID()
{
	DWORD resID=0;
	switch(this->m_iSelectedItem)
	{
	case IDT_INIT:
		resID = IDI_INIT;
		break;
	case IDT_PLAY:
		resID = IDI_SPEAK;
		break;
	case IDT_MAKE_CALL:
		resID = IDI_PHONE;
		break;
	case IDT_TIMECHK:
		resID = IDI_WATCH;
		break;
	case IDT_INPUT:
		resID = IDI_DTMF;
		break;
	case IDT_PLUG:
		resID = IDI_PLUG;
		break;
	case IDT_LOGICAL:
		resID = IDI_ANDOR;
		break;
	case IDT_CONDITION:
		resID = IDI_EQUAL;
		break;
	case IDT_LOOP:
		resID = IDI_CIRCUIT;
		break;
	case IDT_MATH:
		resID = IDI_MATH;
		break;
	case IDT_IS:
		resID = IDI_IS;
		break;
	case IDT_RETRIAL:
		resID = IDI_RETRIAL;
		break;
	case IDT_GOTO:
		resID = IDI_GOTO;
		break;
	case IDT_VOICE_REC:
		resID = IDI_VOICE_REC;
		break;
	case IDT_HUB:
		resID = IDI_HUB;
		break;
	case IDT_PAGE:
		resID = IDI_PAGE;
		break;
	case IDT_CALLERR:
		resID = IDI_CALLERR;
		break;
	}
	return resID;
}
void CScenarioEditorApp::OnFileOpen() 
{
	POSITION pos =GetFirstDocTemplatePosition();
	CDocTemplate *pDocTemplate = this->GetNextDocTemplate(pos);
	while(pDocTemplate)
	{
		pDocTemplate->CloseAllDocuments(TRUE);
		pDocTemplate = this->GetNextDocTemplate(pos);
	}
}

void CScenarioEditorApp::OnFileNew() 
{
	POSITION pos =GetFirstDocTemplatePosition();
	CDocTemplate *pDocTemplate = this->GetNextDocTemplate(pos);
	while(pDocTemplate )
	{
		pDocTemplate->CloseAllDocuments(TRUE);
		pDocTemplate = this->GetNextDocTemplate(pos);
	}
}
