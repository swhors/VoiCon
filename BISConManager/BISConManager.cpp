// BISConManager.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BISConManager.h"
// for common header file.
#include "globaldef.h"
#include "msg_define.h"

#include "MainFrm.h"
#include "BISConManagerDoc.h"

//for license
#include "IVRKeyData.h"
#include "KeyLockCtrlDll.h"

#include "StateView.h"
#include "global.h"

// property sheet page
#include "Properpage1.h"
#include "Properpage2.h"
#include "Properpage3.h"
#include "Properpage4.h"
#include "Properpage5.h"
#include "Properpage6.h"
#include "Properpage7.h"
#include "ProperPageQ931.h"
#include "properpagecodec.h"

// configuration and ftp server
//#include "ioctrl.h"
//#include "ValInterface.h"
//#include "FileInterface.h"
#include <psapi.h>
#include "FTPServer.h"
#include "terminal.h"
#include "ProcessChk.h"

#include "IVRParsor.h"

#include "LoadProgress.h"

#include "ExceptionHandler.h"

#if 0
CLoadProgress pLoadProgress;
#else
CLoadProgress *pLoadProgress=NULL;
#endif

#define RECHK_IVR    100020

BOOL    bLoadingState; // IVR이 로딩 중인지 아닌지를 나타냄.
                        //SendLoadingInfo안에서 사용됨.
BOOL    bLoadingProgressState; // IVR의 로딩 상태를 나타내기 위해서
                                // 프로그래서 바를 사용한지를 나타내는 플래그.

#define SHAREDMEM_SIZE 160
extern CCallStateBuf * pStateBuf;

CBISConManagerApp theApp;
int g_nCallState[200];

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBISConManagerApp

BEGIN_MESSAGE_MAP(CBISConManagerApp, CWinApp)
	//{{AFX_MSG_MAP(CBISConManagerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_TM_START, OnTmStart)
	ON_COMMAND(IDM_TM_STOP, OnTmStop)
	ON_COMMAND(IDM_VOICON_START, OnVoiconStart)
	ON_COMMAND(IDM_VOICON_STOP, OnVoiconStop)
	ON_COMMAND(IDM_CONFIG, OnConfig)
	ON_UPDATE_COMMAND_UI(IDM_TM_START, OnUpdateTmStart)
	ON_UPDATE_COMMAND_UI(IDM_TM_STOP, OnUpdateTmStop)
	ON_UPDATE_COMMAND_UI(IDM_VOICON_START, OnUpdateVoiconStart)
	ON_UPDATE_COMMAND_UI(IDM_VOICON_STOP, OnUpdateVoiconStop)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBISConManagerApp construction

CBISConManagerApp::CBISConManagerApp()
{
    m_pCallInfo        = NULL;
    this->m_pMainFrame = NULL;
    this->m_pStateView = NULL;
    m_nCallCount    = 0;
    m_nIvrIsRun        = 0;
    pStateBuf        = NULL;
}

CBISConManagerApp::~CBISConManagerApp()
{
    if( pStateBuf )
        delete []pStateBuf;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CBISConManagerApp object

/////////////////////////////////////////////////////////////////////////////

int GetKeyLockMaxCall()
{

    int iRetCode = 0;
    int iMaxCall = 0;
    IVRFILELOCK keyData;

    iRetCode = KeyVerifyWithMac(key_auto);
    switch(iRetCode)
    {
    case -1: // 비정상
        MessageBox(NULL, "Data Error [ file or USB device : type 0].", "Error : BIS_IVR", MB_OK);
        return -1;
    case 0: // 빈 라이센스 키임.
        if( !KeySetUser(key_auto) )
        {   // 서버의 맥 주소를 입력.
            MessageBox(NULL, "Data Error [ file or USB device : type 1].", "Error : BIS_IVR", MB_OK);
            return -1;
        }
    case 1:
        {  // 정상적인 데이터.
            if( !KeyRead("license.dat", (PBYTE)&keyData, sizeof(IVRFILELOCK), key_auto) )
            {
                MessageBox(NULL, "Data Error [ file or USB device : type 2].", "Error : BIS_IVR", MB_OK);
                return -1;
            }
            //기간을 검사.
            SYSTEMTIME sysTime;
            ::GetSystemTime(&sysTime);
            char tBuff[64];
            memset(tBuff, 0, 64);
            wsprintf(tBuff, "%04d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
            if( memcmp(tBuff, keyData.szDueDay, strlen(keyData.szDueDay) ) >= 0 )
            {
                return -1;
            }

            iMaxCall = atoi(keyData.szMaxCall);
        }
        break;
    }
    return iMaxCall;
}
 

// CBISConManagerApp initialization
UINT SendUnLoadingInfo(PVOID pArg)
{
    if( theApp.terminal)
        theApp.terminal->SendCallLog("IVR UnLoaing.... \n\r", -1);
    while( theApp.bFindIvrProc() )
    {
        if( theApp.terminal)
            theApp.terminal->SendCallLog("#", -1);
        Sleep(45);
    }
    if( theApp.terminal)
        theApp.terminal->SendCallLog("\n\r", -1);
    return 1;
}

UINT SendLoadingInfo(PVOID pArg)
{
    int iFlag = (int)pArg;
    if( theApp.terminal)
    {
        if( iFlag == 1)
            theApp.terminal->SendCallLog("IVR Loaing.... \n\r", -1);
        else
        {
            CloseHandle(AfxGetThread()->m_hThread);
            return 0;
        }
    }
    while( bLoadingState )
    {
        if( theApp.terminal)
            theApp.terminal->SendCallLog("#", -1);
        Sleep(30);
    }
    if( theApp.terminal)
        theApp.terminal->SendCallLog("\n\r", -1);
    return 0;
}

UINT TerminalSendInfo(PVOID pArg)
{
	char sbuf[SHAREDMEM_SIZE];
	int iCurPos = 0;
    if(theApp.sSharedMemory1)
    {
		for( int i = 0 ; i< theApp.m_pIVR_System.Ivr_MaxCall; i++)
		{
			memset( sbuf, 0, SHAREDMEM_SIZE);
			iCurPos = i * SHAREDMEM_SIZE;
			memcpy(sbuf,theApp.sSharedMemory1+iCurPos, SHAREDMEM_SIZE);
			theApp.m_pParsor->SetCallStatus(sbuf);
			if( theApp.terminal )
				theApp.terminal->SendCallLog(sbuf, theApp.m_pIVR_System.Ivr_MaxCall);
		}
    }
   return 0;
}

void CALLBACK registered_chk(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
    KillTimer(l_hWnd, eEventId);
    theApp.SendRegMessage();
    theApp.bSetTimer = TRUE;

}

void CALLBACK current_call_chk(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
    KillTimer(l_hWnd, eEventId);
	if( theApp.m_pStateView )
		theApp.m_pStateView->UpdateCurrentCall();
	SetTimer(l_hWnd,eEventId, 500, current_call_chk);
}

void CALLBACK	app_start_chk(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{

}

void CALLBACK ReStartIVR(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{

    //HANDLE hEvent = CreateEvent(NULL, FALSE,FALSE,"IVRUPREADY");
    KillTimer(l_hWnd, eEventId);
    
    if( theApp.bFindIvrProc() == FALSE)
    {
        if( !theApp.bForceEnded )
        {
            if( !theApp.bForceDrop)
            {
                if( theApp.bIvrStart )
                {
                    if( AfxIsValidAddress(theApp.terminal, sizeof( CTerminal), TRUE))
                    {
                        theApp.terminal->SendCallLog("500 NORESPONSE.", -1);
                    }
                }
                theApp.bRunIvrProcess();
                SetTimer(l_hWnd, RECHK_IVR, 1000, registered_chk);
            }
        }
    }
    SetTimer(theApp.m_pMainWnd->m_hWnd, eEventId, 10500, ReStartIVR);
}

UINT SetIVRStateFlag(PVOID pArg)
{
    int iFlag = (int )pArg;
    theApp.bIvrStart = iFlag;
    theApp.m_pMainWnd->RedrawWindow();
    //theApp.m_pMainFrame->m_tDialog.RedrawWindow();
    bLoadingState = FALSE;
    if( bLoadingProgressState )
        bLoadingProgressState = FALSE;
    theApp.bEnterInputStateVOICONStart = FALSE;
#if 0
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, "IVRSTOPING");
	if( hEvent )
	{
		WaitForSingleObject( hEvent, 300 );
		CloseHandle(hEvent);
	}
	if( pLoadProgress )
		delete pLoadProgress;
	pLoadProgress = NULL;
#endif
    return 1;
}

BOOL CBISConManagerApp::InitInstance()
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
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	FindAutoStorageDevice();
    if( !InitGlobal())
        return FALSE;
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("BIS_IVR Controller"));
	for(int i = 0; i < 200 ;i++)
		g_nCallState[i] = 0;
	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBISConManagerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CStateView));
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

	/////////////////////////////////////////////////////////////////////////////

    memset( &pProcessInformation, 0, sizeof( PROCESS_INFORMATION));
    
    hMutex = CreateMutex(NULL, FALSE, "BISVOICE");
    
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
    {
        AfxMessageBox("IVR Controller Program Already Exists !! ");
        return FALSE;
    }
    
    m_pParsor    = new CIVRParsor;
    
    bLoadingState = FALSE;
    bLoadingProgressState = FALSE;
    
    SetVariable();
    //swhors
    bStartTerminal = FALSE;
    
    m_pIVR_System.Ivr_MaxCall = GetKeyLockMaxCall();

	if( m_pIVR_System.Ivr_MaxCall > 0 )
	{
		pStateBuf = new CCallStateBuf[m_pIVR_System.Ivr_MaxCall];
	    for(int i = 0; i< m_pIVR_System.Ivr_MaxCall; i++)
		{
			pStateBuf[i].iCallNum = i;
		}
	}
	SetTimer(theApp.m_pMainWnd->m_hWnd,12345, 500, current_call_chk);    
    if( m_pIVR_System.Ivr_MaxCall < 0 )
        m_pIVR_System.Ivr_MaxCall = 0;
    
    if( bInitGlobalMemory() == FALSE)
    {
        AfxMessageBox("Can't Initialize shared memory.");
        return FALSE;
    }
    //
	m_pIVR_System.iAutoStartTerminal = ::GetPrivateProfileInt("SystemBase Setup", "AutoStartTerminal", 0, "./biscon.ini");
    if( m_pIVR_System.iAutoStartTerminal )
    {
        StartTerminalService();
    }
	if( GbFindIvrProc() )
	{
		m_pMainFrame->set_tb_ivr_stop();
	}
	OnFileNew();
	//LoadExceptionFilter();
    return TRUE;
}

#include "DIBitmap.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CBrush m_HollowBrush;
	CDIBitmap m_BackImage;
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnHttp();
	afx_msg void OnBtnMail();
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
	ON_BN_CLICKED(IDC_BTN_HTTP, OnBtnHttp)
	ON_BN_CLICKED(IDC_BTN_MAIL, OnBtnMail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CBISConManagerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnBtnHttp() 
{
	// TODO: Add your control notification handler code here
    ::ShellExecute( GetSafeHwnd(), "open", "http://www.bis-sys.com", NULL, NULL, SW_SHOW );
	
}

void CAboutDlg::OnBtnMail() 
{
	// TODO: Add your control notification handler code here
    ::ShellExecute(GetSafeHwnd(), NULL, "mailto:bis@bis-sys.com", NULL, NULL, SW_SHOW);
 	
}

/////////////////////////////////////////////////////////////////////////////
// CBISConManagerApp message handlers
void CBISConManagerApp::SetCallInfo()
{
    if( m_nCallCount > 0 )
    {
        m_pCallInfo = new CCallInfo [ m_nCallCount];
        m_pIVR_System.Ivr_MaxCall = m_nCallCount;
    }
}

void CBISConManagerApp::ParsePacket(char *packet )
{
    m_pParsor->ParsePacket(packet);
}

void CBISConManagerApp::SetIVRStart()
{
    if( !bSetTimer )
    {
        SetTimer(m_pMainWnd->m_hWnd, 90000, 500, ReStartIVR);
        bSetTimer = TRUE;
    }
    bLoadingState = TRUE;
    AfxBeginThread(SendLoadingInfo, (int*)1);
    bEnterInputStateVOICONStart = TRUE;
    bForceDrop = FALSE;
    bForceEnded = FALSE;
}

void CBISConManagerApp::SetIVRStop()
{
    bForceDrop = TRUE;
    bForceEnded = TRUE;
    bIvrStart = FALSE;
    AfxBeginThread(SendUnLoadingInfo, NULL);
    SendMessage(HWND_BROADCAST, theApp.MessageReq, ACRO_IVR_RES_CL_REQ, 0);

#if 1
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, "IVRSTOPING");
	if( hEvent )
	{
		WaitForSingleObject( hEvent, 400 );
		CloseHandle(hEvent);
	}
#else
    Sleep(1000);
#endif
    m_pMainWnd->RedrawWindow();
    memset( &pProcessInformation, 0, sizeof( PROCESS_INFORMATION));
}

void CBISConManagerApp::SetVariable()
{
    if( bFindIvrProc() )
    {
        bIvrStart = TRUE;
        if( !bSetTimer )
        {
            SetTimer(m_pMainWnd->m_hWnd, 90000, 500, ReStartIVR);
            bSetTimer = TRUE;
        }
        SendRegMessage();
    }
    else
    {
        bIvrStart = FALSE;
    }
    bStartTerminal = FALSE;
    m_pIVR_System.m_RtpPort					=0;
	m_pIVR_System.m_Q931Port				=0;
	m_pIVR_System.m_RASPort					=0;
    m_pIVR_System.Auth_Server_IP			="";
    m_pIVR_System.Auth_Server_Port			=0;
    m_pIVR_System.Capability_TimeOut		=0;
    m_pIVR_System.DTMF_Waiting_TimeOut		=0;
	m_pIVR_System.DTMF_Init_TimeOut			=0;
    m_pIVR_System.E164ID					="";
    m_pIVR_System.GateKeeper_IP				="";
    m_pIVR_System.H245_Channel_TimeOut		=0;
    m_pIVR_System.H245_MasterSlave_TimeOut	=0;
    m_pIVR_System.H323ID					="";
    m_pIVR_System.Ivr_MaxCall				=0;  
    m_pIVR_System.Ivr_Scenario_File			="";
    m_pIVR_System.m_nLangCount				=0;
    m_pIVR_System.m_nSceCount				=0;
    m_pIVR_System.Ivr_Server_IP				="";
    m_pIVR_System.Max_AudioDelay_Jitter		=0;
    m_pIVR_System.Ment_Interval				=0;
    m_pIVR_System.Q931_Connect_TimeOut		=0;
    m_pIVR_System.Q931_Response_TimeOut		=0;
    m_pIVR_System.Ras_MaxFail				=0;
    m_pIVR_System.Ras_MultiCast_Port		=0;
    m_pIVR_System.Ras_MultiCastIP			="";
    m_pIVR_System.Ras_Response_TimeOut		=0;
    m_pIVR_System.iAutoStartTerminal		=0;
}

void CBISConManagerApp::StartTerminalService()
{
    terminal = new CTerminal;
    if( terminal )
    {
        bStartTerminal = TRUE;
        //SetDlgItemText(this->m_pMainWnd->m_hWnd, IDM_TERMINAL, "&Terminal Stop");
    }
    else
    {
        bStartTerminal = FALSE;
        AfxMessageBox("Can't start terminal.");
    }
    m_pMainWnd->RedrawWindow();
//    m_pMainFrame->m_tDialog.RedrawWindow();
}

void CBISConManagerApp::StopTerminalService()
{
    bReleaseGlobalMemory();
    CloseTerminal();
    bStartTerminal = FALSE;
    m_pMainWnd->RedrawWindow();
}

void CBISConManagerApp::OnVoiconOp() 
{
    CString csbuf;
    if( IsFile("exe","./", "BisConRunner") )
    {
        if(!bIvrStart)
            csbuf = "Are you sure? IVR Start?";
        else 
            csbuf = "Are you sure? IVR Stop?";
        if( IDYES == AfxMessageBox(csbuf, MB_YESNO))
        {
            if( bIvrStart)
            {
                SetIVRStop();
                m_pMainFrame->set_tb_ivr_start();
            }
            else
            {
                if( bEnterInputStateVOICONStart )
                {
                    AfxMessageBox("Now IVR is starting. Wait Please.");
                }
                else 
                {
                    SetIVRStart();
                    bLoadingProgressState = TRUE;
#if 0
                    pLoadProgress.DoModal();
#else
					if( pLoadProgress )
						delete pLoadProgress;
					pLoadProgress = new CLoadProgress;
					pLoadProgress->Create(IDD_DLG_PROC);
					pLoadProgress->ShowWindow(SW_SHOW);
#endif
                }
                m_pMainFrame->set_tb_ivr_stop();
            }
        }
    }
    else
    {
        AfxMessageBox("Error : Not exist. Check Software.");
    }
}

void CBISConManagerApp::OnTerminal()
{
    CString csbuf;
    if(!bStartTerminal)
        csbuf = "Are you sure? Terminal Service Start?";
    else 
        csbuf = "Are you sure? Terminal Service Stop?";
    
    if( IDYES == AfxMessageBox(csbuf, MB_YESNO))
    {
        if( !bStartTerminal )
        {
            StartTerminalService();
        }
        else
        {
            StopTerminalService();
        }
    }
}

BOOL CBISConManagerApp::bFindIvrProc()
{
    return GbFindIvrProc();
}

BOOL CBISConManagerApp::bRunIvrProcess()
{
    return GbRunProcess(&pProcessInformation,IVR_PAOGRAM_NAME,bForceDrop);
}

void CBISConManagerApp::SendRegMessage()
{
    ::SendMessage(HWND_BROADCAST, MessageReg, (LONG)m_pMainWnd->m_hWnd, NULL);
}

void CBISConManagerApp::CloseTerminal()
{
	if( terminal )
	{
        for( int i = 0; i< terminal->iConnectCount ; i++)
		{
            terminal->ct[i].Close();
        }
        terminal->Close();
        delete terminal;
        terminal = NULL;
    }
}

BOOL CBISConManagerApp::bInitGlobalMessageHandle()
{
	MessageState	= RegisterWindowMessage(MESSAGE_GLOBAL_STATE);
    MessageReq		= RegisterWindowMessage(MESSAGE_GLOBAL_ACK);
    MessageRes		= RegisterWindowMessage(MESSAGE_GLOBAL_RES);
    MessageReg		= RegisterWindowMessage(MESSAGE_GLOBAL_REG);
    MessageRek		= RegisterWindowMessage(MESSAGE_GLOBAL_REK);
    RECVCALLINFO	= RegisterWindowMessage(MESSAGE_RECVCALLINFO);
    RECVDETAILLOG	= RegisterWindowMessage(MESSAGE_DETAILLOG);
    MessageShow		= RegisterWindowMessage( MESSAGE_MESSAGESHOW );

    if( MessageReq==0 || MessageRes==0 || MessageReg==0 || MessageRek==0 || RECVCALLINFO == 0 || RECVDETAILLOG == 0 || MessageShow == 0 ){
        AfxMessageBox("Error : Can't create global messgae handle.");
        return FALSE;
    }
    return TRUE;
}

BOOL CBISConManagerApp::InitGlobal()
{
   bForceEnded = FALSE;
    hIvrHandle = NULL;
    bForceDrop = FALSE;
    hKeepAlive = NULL;
    bSetTimer = FALSE;
    bEnterInputStateVOICONStart = FALSE;
    if( bInitGlobalMessageHandle() == FALSE )
    {
        AfxMessageBox("Error : Can't create global event message.");
        return FALSE;
    }
    bAutoStart = FALSE; // up 명령어 후에 바로 시작할 것인지를 결정.
    return TRUE;
}

BOOL CBISConManagerApp::bInitGlobalMemory()
{
	hSharedMem1 = ::CreateFileMapping((HANDLE)0XFFFFFFFF,\
		NULL, PAGE_READWRITE, 0, \
		SHAREDMEM_SIZE*m_pIVR_System.Ivr_MaxCall,\
		"AcroIVR-Shared-Global-1");
	
	if( hSharedMem1 == NULL )
	{
		AfxMessageBox("Error : Can't get shared memory handle.[call log]");
		return FALSE;
	}
	
	sSharedMemory1 = (char *)MapViewOfFile( hSharedMem1, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	if( sSharedMemory1 == NULL )
	{
		AfxMessageBox("Error : Can't view of shared memory.[call log]");
		bReleaseGlobalMemory();
		return FALSE;
	}
	return TRUE;
}

BOOL CBISConManagerApp::bReleaseGlobalMemory()
{
    if( sSharedMemory1 )
    {
        UnmapViewOfFile(sSharedMemory1);
        sSharedMemory1 = NULL;
    }
    if( hSharedMem1)
    {
        CloseHandle(hSharedMem1);
        hSharedMem1 = NULL;
    }

    ReleaseMutex( hMutex );
    return TRUE;
}

LRESULT CAboutDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	theApp.IVRMSGProc(message, wParam, lParam);
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CBISConManagerApp::AcroIvrInterProcRes(WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case ACRO_IVR_GETCONSTATE_ACK:
        {
            char state_string[128];
            switch((int)lParam)
            {
            case 0:
                sprintf(state_string, "600 BIS_IVR's ep is initializing.");
                break;
            case 1:
                sprintf(state_string, "600 BIS_IVR's ep is connected to gk.");
                break;
            case 2:
                sprintf(state_string, "600 BIS_IVR's ep is not connected to gk.");
                break;
            case 3:
                sprintf(state_string, "600 BIS_IVR's ep is ending.");
                break;
            }
            if( terminal )
                terminal->SendCallLog(state_string, -1);
        }
        break;
    case ACRO_IVR_GETRUNSTATE_ACK:
        if( terminal)
        {
            if( (BOOL)lParam == TRUE)
            {
                terminal->SendCallLog("BIS_IVR is running.", -1);
            }
            else if ((BOOL)lParam == FALSE)
            {
                terminal->SendCallLog("BIS_IVR is not running.", -1);
            }
            else
            {
                terminal->SendCallLog("BIS_IVR is not running state.", -1);
            }
        }
        break;
    case ACRO_IVR_RUN_ACK:
        if( terminal)
        {
            if( (BOOL)lParam == TRUE)
            {
                terminal->SendCallLog("OK", -1);
            }
            else if((BOOL)lParam == FALSE)
            {
                terminal->SendCallLog("Error : BIS_IVR can't run.", -1);
            }
            else
            {
                terminal->SendCallLog("Error : BIS_IVR is not runnig state.", -1);
            }
        }
        break;
    case ACRO_IVR_STOP_ACK:
        if( terminal)
        {
            if( (BOOL)lParam == TRUE)
            {
                terminal->SendCallLog("Error : BIS_IVR can't stop.", -1);
            }
            else if ((BOOL)lParam == FALSE)
            {
                terminal->SendCallLog("OK", -1);
            }
            else
            {
                terminal->SendCallLog("Error : BIS_IVR is not runnig state.", -1);
            }
        }
        break;
    case ACRO_IVR_DOWN_ACK:
        if( terminal)
        {
            switch((int)lParam)
            {
            case 2:
                terminal->SendCallLog("Error : BIS_IVR is not runnig state.", -1);
                break;
            default :
                terminal->SendCallLog("OK", -1);
                //partner_hWnd = NULL;
                break;
            }
        }
        break;
    case ACRO_IVR_UP_COMPLETE:
        {
			//AfxMessageBox("Complete");
			SetIVRStateFlag((int*)1);
			m_pMainFrame->set_tb_ivr_stop();
        }
        break;
    case ACRO_IVR_UP_ACK:
        if( terminal)
        {
            switch((int)lParam)
            {
            case 0: // can't run.
                terminal->SendCallLog("Error : Can't run BIS_IVR.", -1);
                break;
            case 1: // ok
                terminal->SendCallLog("OK", -1);
                break;
            case 2: // already run.
                terminal->SendCallLog("Error : Already run.", -1);
                m_pMainFrame->set_tb_ivr_start();
                break;
            }
        }
        break;
    case ACRO_IVR_RES_CL_ACK:
        if( bFindIvrProc())
        {
            bForceEnded = TRUE;
            if( pProcessInformation.hProcess )
            {
                TerminateProcess(pProcessInformation.hProcess, 0);
                if( AfxIsValidAddress(terminal, sizeof( CTerminal ), TRUE))
                {
                    terminal->SendCallLog("OK", -1);
                    terminal->SendCallLog("500 NORESPONSE.", -1);
                }
                CloseHandle(pProcessInformation.hProcess);
            }
        }
        break;
    }
}

void CBISConManagerApp::IVRMSGProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if( message == MessageRes)
    {
        AcroIvrInterProcRes(wParam, lParam);
    }
    
    if( message == RECVCALLINFO)
    {
        iCallNum = (int)lParam;
        TerminalSendInfo(this);
    }
    if( message == MessageReg || message == MessageRek)
    {
        HWND m_hWnd = m_pMainWnd->m_hWnd;
        if( m_hWnd != (HWND) wParam)
        {
		//    partner_hWnd = (HWND) wParam;
		//    AfxBeginThread(SetIVRStateFlag, (int*)1);
		//    //SetIVRStateFlag((int*)1);
            if( message == MessageReg)
            {
                ::SendMessage(HWND_BROADCAST, MessageRek, (long)m_hWnd, 0);
                if( terminal)
                {
                    terminal->SendCallLog("400 IVR completed upload.", -1);
                    terminal->SendCallLog("OK", -1);
                }
                bForceEnded = FALSE;
            }
        }
    }
	else if( message == MessageReq)
	{
		if( wParam == ACRO_IVR_MAXCALL_SEND )
		{
			if( this->m_pStateView )
			{
				char tbuf[32];
				wsprintf(tbuf, "%09d", (UINT)lParam);
				this->m_pStateView->m_edTotalCount.SetWindowText(tbuf);
			}
			// 전체 콜 수.
		}
		else if( wParam == ACRO_IVR_REGISTERED_FLAG )
		{
			if( (BOOL)lParam == FALSE)
			{
				if( this->m_pMainFrame )
				{
					//this->m_pMainFrame->m_wndStatusBar.SetPaneText(1, "IVR OFF");
					this->m_pMainFrame->SetStatusBarPaneText(1, "IVR OFF");
					//this->m_pMainFrame->csBarString = "UnReady";
				}
			}
			else
			{
				if( this->m_pMainFrame )
				{
					this->m_pMainFrame->SetStatusBarPaneText(1, "IVR ON");
					//this->m_pMainFrame->m_wndStatusBar.SetPaneText(1, "IVR ON");
					//this->m_pMainFrame->csBarString = "Ready";
				}
			}
			// IVR의 등록 여부
		}
	}
	else if(MessageState)
	{
		if( this->m_pMainFrame )
			this->m_pMainFrame->m_bIVRRegistered = (int)lParam;
		//SendMessage(HWND_BROADCAST, MessageState, NULL, (LPARAM) m_bRegisteredBISCon);
	}	
}

void CBISConManagerApp::OnConfig() 
{
	// TODO: Add your command handler code here
	this->ConfDoModal();
}

void CBISConManagerApp::OnTmStart() 
{
	// TODO: Add your command handler code here
   this->StartTerminalService();
	
}

void CBISConManagerApp::OnUpdateTmStart(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
   if(this->bStartTerminal)
        this->m_pMainFrame->DisableMenuItem(pCmdUI->m_nID);
    else
        this->m_pMainFrame->EnableMenuItem(pCmdUI->m_nID);
	
}

void CBISConManagerApp::OnTmStop() 
{
	// TODO: Add your command handler code here
    this->StopTerminalService();
	
}

void CBISConManagerApp::OnUpdateTmStop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
   if(this->bStartTerminal)
        this->m_pMainFrame->EnableMenuItem(pCmdUI->m_nID);
    else
        this->m_pMainFrame->DisableMenuItem(pCmdUI->m_nID);
}

void CBISConManagerApp::OnVoiconStart() 
{
	// TODO: Add your command handler code here
    OnVoiconOp();
	
}

void CBISConManagerApp::OnUpdateVoiconStart(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    if(this->bIvrStart)
        this->m_pMainFrame->DisableMenuItem(pCmdUI->m_nID);
    else
        this->m_pMainFrame->EnableMenuItem(pCmdUI->m_nID);
    SetToolBar(pCmdUI);
	
}

void CBISConManagerApp::OnUpdateVoiconStop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    if(this->bIvrStart)
        this->m_pMainFrame->EnableMenuItem(pCmdUI->m_nID);
    else
        this->m_pMainFrame->DisableMenuItem(pCmdUI->m_nID);
    SetToolBar(pCmdUI);
	
}

void CBISConManagerApp::OnVoiconStop() 
{
	// TODO: Add your command handler code here
    OnVoiconOp();
	
}

void CBISConManagerApp::SetToolBar(CCmdUI *pCmdUI)
{
    if(!bIvrStart)
        this->m_pMainFrame->set_tb_ivr_start();
    else
        this->m_pMainFrame->set_tb_ivr_stop();
}

void CBISConManagerApp::ConfDoModal()
{
    CProperPage1 page1;
    CProperPage2 page2;
    CProperPage3 page3;
    //CProperPage4 page4;
    //CProperPage5 page5;
    CProperPage6 page6;
    //CProperPage7 page7;
	CProperPageQ931 page8;
	CProperPageCodec page9;
    
    CPropertySheet pDlg("Setup Box");
    
    pDlg.AddPage(&page1);
    pDlg.AddPage(&page2);
    pDlg.AddPage(&page3);
    //pDlg.AddPage(&page4);
    //pDlg.AddPage(&page5);
    pDlg.AddPage(&page6);
    //pDlg.AddPage(&page7);
	pDlg.AddPage(&page8);
	pDlg.AddPage(&page9);
    pDlg.DoModal();
}
