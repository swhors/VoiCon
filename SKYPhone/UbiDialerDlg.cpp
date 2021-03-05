// UbiDialerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UbiDialer.h"
#include "UbiDialerDlg.h"
//#include "LoginDlg.h"
#include "PhoneBookDlg.h"
#include "RecentTalkDlg.h"
#include "SCUsbPhone.h"
#include "SmsMainDlg.h"
//#include "UBIConfDlg.h"
#include "UBIConf.h"
#include "SkyChat.h"
#ifdef SIP_UA
#include "sipmain.h"
#include "SkySock.h"
#include "skysipua_g.h"
#elif defined H323_UA
#include "cm.h"
#endif
#include "BlowFish.h"
#include "H323CallInfo.h"
#include "WaveCtrl.h"

#include "RadMiscFunction.h"

#include "wmsg_define.h"

HAPP hApp;

// hid buzz
extern "C"
{
	#include "setupapi.h"
	#include "hidsdi.h"
}

#include "resource.h"

#include "YLUSBTEL.h"

#include "MyAudioMixer.h"

#include "operate.h"

static BOOL bForeceEnd;

extern HWND gMediaWnd;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UM_REMAIN_RCV WM_USER + 111
#define NO_SOURCE	NULL

#define	WM_CHANGE_NOTIFY   WM_USER+2570
#define NOWTIME_TIME	   1010101

#define CHK_USB_SND 30001

typedef BOOL YLDEVIOCTRL(DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD);

extern int busycount;

extern int		m_g729Support;
extern int		m_InitOfRtpPort;
extern BOOL	m_bRegisteredBISCon;


#ifdef SIP_UA
extern SKYLIB int sip_error_code;
#endif

BOOL bFirst;

CRITICAL_SECTION endProcCritical;

CUbiDialerDlg	*g_PhoneDlg;
CSkyChat *chatdlg;

int				m_nMixerID = 0;
DWORD			m_dwMicSource = 1;
WSADATA			gszWSAData;

DWORD dwErrorNo;

int shift_down;

void YLUSBTELCallBack(LPVOID wParam,LPVOID lParam,DWORD ParameterOfApplications)
{
	CUbiDialerDlg *pThis = (CUbiDialerDlg *)ParameterOfApplications;
	pThis->PostMessage(WM_CHANGE_NOTIFY,(WPARAM)wParam,(LPARAM)lParam);
}

int SetPrivateProfileString(IN char *index,IN char *item,IN char *Val,IN int len,IN char *filename)
{
	//::WritePrivateProfileString(lpAppName, lpKeyName, lpString, lpFileName);
	//::GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
	return WritePrivateProfileString(PHONE_NAME, item, Val, SIPCALLER_CONF);

}
int SetPrivateProfileInt(IN char *index,IN char *item,IN int iVal,IN char *filename)
{
	char tbuf[32];
	sprintf(tbuf, "%d", iVal);
	return WritePrivateProfileString(PHONE_NAME, item, tbuf, SIPCALLER_CONF);
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

/////////////////////////////////////////////////////////////////////////////
// CUbiDialerDlg dialog

CUbiDialerDlg::CUbiDialerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUbiDialerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUbiDialerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#ifdef SIP_UA
	m_pPhone = NULL;
#endif
	char        szExeDir[128];

    GetExeDir(szExeDir, 128);
	m_sExeDir.Format("%s",szExeDir);	
	APP->m_pUbiDialerDlg = this;	
	memset( m_szPhone, 0, sizeof(m_szPhone) );

	bMicMute = FALSE;
	bSpeakerMute = FALSE;
	InitializeCriticalSection(&endProcCritical);
	bFirst = FALSE;
}

CUbiDialerDlg::~CUbiDialerDlg()
{
	DeleteCriticalSection(&endProcCritical);
}

void CUbiDialerDlg::GetExeDir(LPSTR lpCurDir, DWORD dwSize)
{
    // 실행 화일의 디렉토리를 가져오는 함수.
    GetModuleFileName(AfxGetInstanceHandle(), lpCurDir, dwSize);

    // 실행 화일명을 띄어 내고 디렉토리만을 추출
    for (int n = lstrlen(lpCurDir) - 1; n >= 0; n --)
    {
        if (*(lpCurDir + n) == '\\')
        {
            *(lpCurDir + n) = 0x00;
            return;
        }
    }
}

void CUbiDialerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUbiDialerDlg)
	DDX_Control(pDX, IDC_BTN_SMS, m_btnSms);
	DDX_Control(pDX, IDC_LABEL_DURATION, m_lblDur);
	DDX_Control(pDX, IDC_LABEL_AMOUNT, m_lblAmount);
	DDX_Control(pDX, IDC_BTN_TEL_HST, m_btnHistory);
	DDX_Control(pDX, IDC_BTN_BOOK, m_btnBook);
	DDX_Control(pDX, IDC_SPK_SLIDE, m_sldSpk);
	DDX_Control(pDX, IDC_MIC_SLIDE, m_sldMic);
	DDX_Control(pDX, IDC_CALLER_LABEL, m_lblCallerNo);
	DDX_Control(pDX, IDC_TIME_LABEL, m_lblTime);
	DDX_Control(pDX, IDC_MSG_LABEL, m_lblMsg);
	DDX_Control(pDX, IDC_MID_LABEL, m_lblMid);
	DDX_Control(pDX, IDC_MIC_IMAGE, m_imgMic);
	DDX_Control(pDX, IDC_SPK_IMAGE, m_imgSpk);
	DDX_Control(pDX, IDC_BTN_MINI, m_btnMini);
	DDX_Control(pDX, IDC_BTN_HELP, m_btnHelp);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_ST, m_btnStar);
	DDX_Control(pDX, IDC_BTN_SH, m_btnSharp);
	DDX_Control(pDX, IDC_BTN_9, m_btn9);
	DDX_Control(pDX, IDC_BTN_8, m_btn8);
	DDX_Control(pDX, IDC_BTN_7, m_btn7);
	DDX_Control(pDX, IDC_BTN_6, m_btn6);
	DDX_Control(pDX, IDC_BTN_5, m_btn5);
	DDX_Control(pDX, IDC_BTN_4, m_btn4);
	DDX_Control(pDX, IDC_BTN_3, m_btn3);
	DDX_Control(pDX, IDC_BTN_2, m_btn2);
	DDX_Control(pDX, IDC_BTN_1, m_btn1);
	DDX_Control(pDX, IDC_BTN_0, m_btn0);
	DDX_Control(pDX, IDC_BTN_SEND, m_btnSend);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_BTN_END, m_btnEnd);	
	DDX_Control(pDX, IDC_BTN_CONF, m_btnConf);
	DDX_Control(pDX, IDC_BTN_ANT, m_antena);
	DDX_Control(pDX, IDC_BTN_CHAT, m_btnChat);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BTN_TRANSFER, m_btnTransfer);
	DDX_Control(pDX, IDC_BTN_RECHARGE, m_recharge);
}

BEGIN_MESSAGE_MAP(CUbiDialerDlg, CDialog)
	//{{AFX_MSG_MAP(CUbiDialerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_BTN_0, OnBtn0)
	ON_BN_CLICKED(IDC_BTN_1, OnBtn1)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_MID_LABEL, OnMidLabel)
	ON_BN_CLICKED(IDC_BTN_2, OnBtn2)
	ON_BN_CLICKED(IDC_BTN_3, OnBtn3)
	ON_BN_CLICKED(IDC_BTN_4, OnBtn4)
	ON_BN_CLICKED(IDC_BTN_5, OnBtn5)
	ON_BN_CLICKED(IDC_BTN_6, OnBtn6)
	ON_BN_CLICKED(IDC_BTN_7, OnBtn7)
	ON_BN_CLICKED(IDC_BTN_8, OnBtn8)
	ON_BN_CLICKED(IDC_BTN_9, OnBtn9)
	ON_BN_CLICKED(IDC_BTN_ST, OnBtnSt)
	ON_BN_CLICKED(IDC_BTN_SH, OnBtnSh)
	ON_BN_CLICKED(IDC_BTN_END, OnBtnEnd)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_MINI, OnBtnMini)
	ON_BN_CLICKED(IDC_MIC_IMAGE, OnMicImage)
	ON_BN_CLICKED(IDC_SPK_IMAGE, OnSpkImage)
	ON_BN_CLICKED(IDC_BTN_BOOK, OnBtnBook)
	ON_BN_CLICKED(IDC_BTN_TEL_HST, OnBtnTelHst)
	ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
	ON_MESSAGE(SL_CHANGE_SPK_VAL, OnSlideChangeSpkVal)
	ON_MESSAGE(SL_CHANGE_MIC_VAL, OnSlideChangeMicVal)
	ON_MESSAGE( UM_REMAIN_RCV, OnRemainRecv)
	ON_MESSAGE(WM_USER_MSG, OnWmUserMsg)
	ON_MESSAGE(WM_CHANGE_NOTIFY, IOSendMessage)
	ON_MESSAGE(WM_CHAT_MSGSD, OnWmChatSend)
	ON_MESSAGE(WM_CHAT_DESTROY, OnChatDlgDown)
	ON_BN_CLICKED(IDC_BTN_SMS, OnBtnSms)
	ON_MESSAGE(WM_USB_CLOSE_MSG, OnWmDownUSB)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_CHAT, OnBnClickedBtnChat)
	ON_BN_CLICKED(IDC_LABEL_AMOUNT, OnBnClickedLabelAmount)
	ON_BN_CLICKED(IDC_LABEL_DURATION, OnBnClickedLabelDuration)
	ON_BN_CLICKED(IDC_BTN_CONF, OnBnClickedBtnConf)
	ON_WM_TIMER()
	ON_WM_DESTROY()
//	ON_BN_CLICKED(IDC_BTN_TRANSFER, OnBnClickedBtnTransfer)
	ON_BN_CLICKED(IDC_BTN_RECHARGE, OnBnClickedBtnRecharge)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUbiDialerDlg message handlers

int CUbiDialerDlg::check_time_lock()
{
	SYSTEMTIME tt;
	GetLocalTime(&tt);
	if( tt.wYear != 2006 )
	{
		return 0;
	}
	if( tt.wMonth > 2)
	{
		return 0;
	}
    return 1;
}

BOOL CUbiDialerDlg::OnInitDialog()
{
	int i;
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
    chatdlg = NULL;
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	hidWriteDevice = NULL;
	// TODO: Add extra initialization here
	m_bConnect = 0;
	bCalling = 0;
	bCalled = 0;
	
	// USB device.
	LoadUSBDeviceDll();
	SetTimer(1, 500, NULL);
	this->m_CallConf.init();
	if( !start_ua() )
	{
		CDialog::OnCancel();
		return FALSE;
	}
    g_PhoneDlg = this;
	for( i = 0; i < 10 ; i++)
	{
		memset( m_szUsedNumber[i], 0, 64);
	}
	iLastUsedNumber = 0;
	iCurUsedNumber = 0;
	memset( &TalkBuff, 0, sizeof( t_Talk ) );
#ifdef SIP_UA
	if ( m_strID.GetLength() > 1 && m_strPN.GetLength() > 1 )
	{
		// 초기화및 시작
		m_pPhone->PInit();
		m_pPhone->RemainCharge( 1 );
	}
	else	AfxMessageBox( "H323ID 와 E.164가 없습니다.\r\n관리자에게 문의하여 주시기 바랍니다.!!!");
#elif defined H323_UA
	// 사용하지 않음.
#endif

#ifdef _USE_AUTO_UP
#if 1
	int ret = CheckUpdate();
#else
	int ret = 1;
#endif
	if( !ret || (ret == 2))
	{
		if( ret == 2)
			AfxMessageBox("Request to Update this program.");
		stop_sipua(0);
		Sleep(1000);
		CDialog::OnCancel();
		return FALSE;
	}
#endif // _USE_AUTO_UP

	// 차후에 추가할 것..
	//m_pPhone->GetRemain( m_strAmount, m_strTime );
    SetTimer(NOWTIME_TIME, 10000,NULL);
	InitSkin();	
	GetNowTime();
	shift_down = 0;

	GetWaveInVolume(); // 현재 마이크 볼륨 크기를 가져와 스라이드바에 표시
	GetWaveOutVolume();	// 마찮가지 wave out 불륨
	hid_buzz_on();
	bForeceEnd = FALSE;
	SetTimer(CHK_USB_SND, 30,NULL);
	//this->Set
	SetFocus();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUbiDialerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUbiDialerDlg::ViewAntena()
{
#ifdef SIP_UA
	if( m_pPhone )
#endif
	{
		CBitmap bit;
#ifdef SIP_UA
		if( m_pPhone->bRegistered )
			bit.LoadBitmap(IDB_BITMAP_ANT_5);
		else
			bit.LoadBitmap(IDB_BITMAP_ANT_0);
#else if H323_UA
		if( m_bRegisteredBISCon )
			bit.LoadBitmap(IDB_BITMAP_ANT_5);
		else
			bit.LoadBitmap(IDB_BITMAP_ANT_0);
#endif
		m_antena.SetBitmap(bit);
		m_antena.ShowWindow(SW_SHOW);
		m_antena.UpdateWindow();
	}
}

void CUbiDialerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
        
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
        
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
        
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		ViewAntena();
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUbiDialerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

////////////////////////////////////////////////////////////////////////////////////
void CUbiDialerDlg::InitSkinFont()
{
	CString path;

	m_TopFont.SetChGapPixel( 2 );
	m_TopFont.SetFontCh( '0', 6, 10, IDB_BITMAP_TOP_0);
	m_TopFont.SetFontCh( '1', 6, 10, IDB_BITMAP_TOP_1);
	m_TopFont.SetFontCh( '2', 6, 10, IDB_BITMAP_TOP_2);
	m_TopFont.SetFontCh( '3', 6, 10, IDB_BITMAP_TOP_3);
	m_TopFont.SetFontCh( '4', 6, 10, IDB_BITMAP_TOP_4);
	m_TopFont.SetFontCh( '5', 6, 10, IDB_BITMAP_TOP_5);
	m_TopFont.SetFontCh( '6', 6, 10, IDB_BITMAP_TOP_6);
	m_TopFont.SetFontCh( '7', 6, 10, IDB_BITMAP_TOP_7);
	m_TopFont.SetFontCh( '8', 6, 10, IDB_BITMAP_TOP_8);
	m_TopFont.SetFontCh( '9', 6, 10, IDB_BITMAP_TOP_9);
	m_TopFont.SetFontCh( '-', 6, 10, IDB_BITMAP_TOP_SL);
	m_TopFont.SetFontCh( '*', 6, 10, IDB_BITMAP_TOP_ST);
	m_TopFont.SetFontCh( '#', 6, 10, IDB_BITMAP_TOP_SH);
	m_TopFont.SetFontCh( ':', 4, 10, IDB_BITMAP_TOP_COL);
	m_TopFont.SetFontCh( 'A', 15, 11, IDB_BITMAP_TOP_AM);
	m_TopFont.SetFontCh( 'P', 15, 11, IDB_BITMAP_TOP_PM);
	m_TopFont.SetFontCh( 'u', 6, 14, IDB_BITMAP_ANT_0);
	m_TopFont.SetFontCh( 'v', 6, 14, IDB_BITMAP_ANT_1);
	m_TopFont.SetFontCh( 'w', 6, 14, IDB_BITMAP_ANT_2);
	m_TopFont.SetFontCh( 'x', 6, 14, IDB_BITMAP_ANT_3);
	m_TopFont.SetFontCh( 'y', 6, 14, IDB_BITMAP_ANT_4);
	m_TopFont.SetFontCh( 'z', 6, 14, IDB_BITMAP_ANT_5);
	
	memcpy( &m_TimeFont, &m_TopFont, sizeof( m_TopFont ) );
	m_TimeFont.SetChGapPixel( 1 );

	m_MidFont.SetChGapPixel( 2 );
	m_MidFont.SetFontCh( '0', 10, 16, IDB_BITMAP_MID_0);
	m_MidFont.SetFontCh( '1', 10, 16, IDB_BITMAP_MID_1);
	m_MidFont.SetFontCh( '2', 10, 16, IDB_BITMAP_MID_2);
	m_MidFont.SetFontCh( '3', 10, 16, IDB_BITMAP_MID_3);
	m_MidFont.SetFontCh( '4', 10, 16, IDB_BITMAP_MID_4);
	m_MidFont.SetFontCh( '5', 10, 16, IDB_BITMAP_MID_5);
	m_MidFont.SetFontCh( '6', 10, 16, IDB_BITMAP_MID_6);
	m_MidFont.SetFontCh( '7', 10, 16, IDB_BITMAP_MID_7);
	m_MidFont.SetFontCh( '8', 10, 16, IDB_BITMAP_MID_8);
	m_MidFont.SetFontCh( '9', 10, 16, IDB_BITMAP_MID_9);
	m_MidFont.SetFontCh( '-', 10, 16, IDB_BITMAP_MID_SLA);
	m_MidFont.SetFontCh( '*', 10, 16, IDB_BITMAP_MID_ST);
	m_MidFont.SetFontCh( '#', 10, 16, IDB_BITMAP_MID_SH);
    
	m_DownFont.SetChGapPixel( 1 );
	m_DownFont.SetFontCh( '0', 7, 10, IDB_BITMAP_DOWN_0);
	m_DownFont.SetFontCh( '1', 7, 10, IDB_BITMAP_DOWN_1);
	m_DownFont.SetFontCh( '2', 7, 10, IDB_BITMAP_DOWN_2);
	m_DownFont.SetFontCh( '3', 7, 10, IDB_BITMAP_DOWN_3);
	m_DownFont.SetFontCh( '4', 7, 10, IDB_BITMAP_DOWN_4);
	m_DownFont.SetFontCh( '5', 7, 10, IDB_BITMAP_DOWN_5);
	m_DownFont.SetFontCh( '6', 7, 10, IDB_BITMAP_DOWN_6);
	m_DownFont.SetFontCh( '7', 7, 10, IDB_BITMAP_DOWN_7);
	m_DownFont.SetFontCh( '8', 7, 10, IDB_BITMAP_DOWN_8);
	m_DownFont.SetFontCh( '9', 7, 10, IDB_BITMAP_DOWN_9);
	m_DownFont.SetFontCh( '-', 7, 10, IDB_BITMAP_DOWN_SLA);
	m_DownFont.SetFontCh( '*', 7, 10, IDB_BITMAP_DOWN_ST);
	m_DownFont.SetFontCh( '#', 7, 10, IDB_BITMAP_DOWN_SH);
    
	strcpy( m_TextFont.FontName, "System" );
	m_TextFont.size = 10;
}

void CUbiDialerDlg::InitSkin()
{
	SetWindowText("UBI Phone");

	InitSkinFont();

	CString path, path2, path3;
	path.Format( "%s\\01_00_bg.bmp", MAIN_BG_DIR );

	InitSkinBkGnd( (char*)(LPCTSTR)path );
//help
	path.Format( "%s\\01_01_01_help", MAIN_TOP_DIR );
	InitSkinBtn( m_btnHelp, 174, 3, 15, 13, path );
//mini
	path.Format( "%s\\01_01_02_mini", MAIN_TOP_DIR );
	InitSkinBtn( m_btnMini, 190, 3, 15, 13, path );			
//close
	path.Format( "%s\\01_01_03_close", MAIN_TOP_DIR );
	InitSkinBtn( m_btnClose, 206, 3, 15, 13, path );		
// antena
    //InitSkinLabel( m_antena, 49, 78, 20, 16, &m_TopFont ); // 62/78 93
	//m_antena.AddString("u");
	m_antena.MoveWindow(49, 78, 20, 16);

// transfer
	path.Format( "%s\\01_08_02_trans", MAIN_ADD_DIR );
	InitSkinBtn( m_btnTransfer, 89, 452, 70, 20, path );	// 160/452 160/471

// Recent Talk
	path.Format( "%s\\01_02_02_recent", MAIN_MENU_DIR );
	InitSkinBtn( m_btnHistory, 62, 38, 30, 29, path );	

// recharge 
	path.Format( "%s\\01_02_04_charge", MAIN_MENU_DIR );
	InitSkinBtn( m_recharge, 126, 38, 30, 29, path );	// 160/452 160/471

// Phone Book
	path.Format( "%s\\01_02_03_pbook", MAIN_MENU_DIR );
	InitSkinBtn( m_btnBook, 94, 38, 30, 29, path );

// Setup
	path.Format( "%s\\01_02_01_setup", MAIN_MENU_DIR );
	InitSkinBtn( m_btnConf, 32, 38, 30, 29, path);

// Sms
	path.Format( "%s\\01_03_02_sms", MAIN_MMS_DIR );
	InitSkinBtn( m_btnSms, 95, 226, 61, 41, path );

	// chat
	path.Format( "%s\\01_03_03_chat", MAIN_MMS_DIR );
	InitSkinBtn( m_btnChat, 160, 226, 61, 41, path );

//Send
	path.Format( "%s\\01_06_01_send", MAIN_CALL_DIR );	
	InitSkinBtn( m_btnSend, 21, 277, 74, 41, path );
//Clear
	path.Format( "%s\\01_06_02_clear", MAIN_CALL_DIR );
	InitSkinBtn( m_btnClear, 92, 277, 66, 33, path );
//End
	path.Format( "%s\\01_06_03_end", MAIN_CALL_DIR );
	InitSkinBtn( m_btnEnd, 154, 277, 74, 41, path );
//1
	path.Format( "%s\\01_07_1", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn1, 44, 327, 52, 26, path );
//2
	path.Format( "%s\\01_07_2", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn2, 99, 327, 52, 26, path );
//3
	path.Format( "%s\\01_07_3", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn3, 154, 327, 52, 26, path );
//4
	path.Format( "%s\\01_07_4", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn4, 44, 357, 52, 26, path );
//5
	path.Format( "%s\\01_07_5", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn5, 99, 357, 52, 26, path );
//6
	path.Format( "%s\\01_07_6", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn6, 154, 357, 52, 26, path );
//7
	path.Format( "%s\\01_07_7", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn7, 44, 387, 52, 26, path );
//8
	path.Format( "%s\\01_07_8", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn8, 99, 387, 52, 26, path );
//9
	path.Format( "%s\\01_07_9", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn9, 154, 387, 52, 26, path );
//*
	path.Format( "%s\\01_07_st", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btnStar, 44, 417, 52, 26, path );
//0
	path.Format( "%s\\01_07_0", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btn0, 99, 417, 52, 26, path );
//#
	path.Format( "%s\\01_07_sh", MAIN_NUMBER_DIR );
	InitSkinBtn( m_btnSharp, 154, 417, 52, 26, path );
	
	path.Format( "%s\\01_09_01_mi_on", MAIN_VOLUME_DIR );
	InitSkinBtn( m_imgMic, 8, 451, 22, 23, path );	

	path.Format( "%s\\01_09_02_sp_on", MAIN_VOLUME_DIR );
	InitSkinBtn( m_imgSpk, 220, 451, 22, 23, path );	
    
	InitSkinLabel( m_lblMsg, 42, 123, 168, 20, &m_TextFont );
    m_lblMsg.SetWindowText("trying to register");
	CtlUI(FALSE);
	//m_lblMsg.SetLabel(">>SKYCOM Phone is the Best!!!");
    
	InitSkinLabel( m_lblCallerNo, 66, 83, 100, 10, &m_TopFont );
	m_lblCallerNo.AddString( m_strPN );
    
	InitSkinLabel( m_lblTime, 84, 83, 214, 10, &m_TimeFont );
	InitSkinLabel( m_lblMid, 10, 143, 230, 16, &m_MidFont );
	m_lblMid.SetMaxLength( 15 );
	
// Mic Slide
	path.Format( "%s\\01_09_01_mi_bar_03.bmp", MAIN_VOLUME_DIR );
	path2.Format( "%s\\01_09_01_mi_bar_02.bmp", MAIN_VOLUME_DIR );
	path3.Format( "%s\\01_09_01_mi_bar_01.bmp", MAIN_VOLUME_DIR );
	m_sldMic.SetType( 0 );
	InitSkinSlide( m_sldMic, 20, 329, 5, 109, path, 3, path2, 3, path3, 4 );

	//m_sldMic.SetRange( 0, 0xFFFF );
	//m_sldMic.MoveWindow(20, 329, 5, 109);
	m_sldMic.SetValue(30);
	
// Spk Slide
	path.Format( "%s\\01_09_02_sp_bar_03.bmp", MAIN_VOLUME_DIR );
	path2.Format( "%s\\01_09_02_sp_bar_02.bmp", MAIN_VOLUME_DIR );
	path3.Format( "%s\\01_09_02_sp_bar_01.bmp", MAIN_VOLUME_DIR );
	m_sldSpk.SetType( 1 );
	InitSkinSlide( m_sldSpk, 226, 329, 5, 109, path, 3, path2, 3, path3, 4 );
	//m_sldSpk.SetRange( 0, 0xFFFF );
	//m_sldSpk.MoveWindow(226, 329, 5, 109); // m_sldMic


	m_sldSpk.SetValue(70);
	InitSkinLabel( m_lblAmount, 54, 200, 60, 10, &m_DownFont );
	InitSkinLabel( m_lblDur, 144, 200, 60, 10, &m_DownFont );
	
	m_lblAmount.SetTextAlign(TA_LEFT);
	m_lblAmount.SetLabel( m_strAmount );
	m_lblDur.SetTextAlign(TA_LEFT);
	m_lblDur.SetLabel( m_strTime );

}

void CUbiDialerDlg::InitSkinBkGnd( char *BkGndFile )
{
	LoadBitmapFile( m_bmpBkGnd, BkGndFile );

	BITMAP bmp;
	m_bmpBkGnd.GetBitmap( &bmp );

	SetWindowPos( NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOZORDER );
	BitmapRgn( m_hWnd, (HBITMAP)m_bmpBkGnd, RGB( 255,0,255 ) );
	
	CenterWindow();
	Invalidate();
}

void CUbiDialerDlg::InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, CString &FileName )
{
	CBitmap	*p_bmpBtnUp = new CBitmap;
	CBitmap *p_bmpBtnDn = new CBitmap;
	CBitmap *p_bmpBtnOv = new CBitmap;
		
	char	szUpFile[100], szOverFile[100], szDownFile[100];
	
	sprintf( szUpFile, "%s_n.bmp", FileName );
	sprintf( szOverFile, "%s_o.bmp", FileName );
	sprintf( szDownFile, "%s_p.bmp", FileName );

	LoadBitmapFile( *p_bmpBtnUp, szUpFile);
	LoadBitmapFile( *p_bmpBtnOv, szOverFile);
	LoadBitmapFile( *p_bmpBtnDn, szDownFile);

    UINT nStyle = Btn.GetButtonStyle();
    Btn.SetButtonStyle(nStyle | BS_OWNERDRAW);    // OWNERDRAW 속성 추가

	Btn.LoadBitmaps( p_bmpBtnUp, p_bmpBtnDn, p_bmpBtnOv );

	Btn.SizeToContent( x, y, w, h, TRUE );
}

void CUbiDialerDlg::InitSkinSlide( CSkinSlide &Slide, int x, int y, int w, int h, 
			CString &HeadFile, int Head_h, CString &BodyFile, int Body_h, CString &TailFile, int Tail_h )
{
    UINT nStyle = Slide.GetButtonStyle();
    Slide.SetButtonStyle(nStyle | BS_OWNERDRAW);    // OWNERDRAW 속성 추가
	
	Slide.SetHeadBmp( (char*)(LPCTSTR)HeadFile, Head_h );
	Slide.SetBodyBmp( (char*)(LPCTSTR)BodyFile, Body_h );
	Slide.SetTailBmp( (char*)(LPCTSTR)TailFile, Tail_h );
	
	Slide.SizeToContent( x, y, w, h, TRUE );
}

void CUbiDialerDlg::InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, CSkinFont *SkinFont )
{
    UINT nStyle = Label.GetButtonStyle();
    Label.SetButtonStyle(nStyle | BS_OWNERDRAW);    // OWNERDRAW 속성 추가

	Label.SetSkinFont( SkinFont );
	Label.SizeToContent( x, y, w, h, TRUE );
}

void CUbiDialerDlg::InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, t_Font *SkinFont )
{
    UINT nStyle = Label.GetButtonStyle();
    Label.SetButtonStyle(nStyle | BS_OWNERDRAW);    // OWNERDRAW 속성 추가

	Label.SetSkinFont( SkinFont );
	Label.SizeToContent( x, y, w, h, TRUE );
}

BOOL CUbiDialerDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	CRect rc;
	GetClientRect(&rc);

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);

	CBitmap* pbmpOld = MemDC.SelectObject( &m_bmpBkGnd );
	pDC->BitBlt(0, 0, rc.right, rc.bottom, &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject( pbmpOld );
	ReleaseDC(pDC);

	return TRUE;
}

// Phone 이동 Clinet -> Caption으로 인식
UINT CUbiDialerDlg::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	UINT nReturn = CDialog::OnNcHitTest(point);
    
    if(nReturn == HTCLIENT)
		nReturn = HTCAPTION;  
    return nReturn;  
}


HBRUSH CUbiDialerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	//m_sldSpk.MoveWindow(226, 329, 5, 109); // m_sldMic
    //if ( pWnd != &m_sldSpk && pWnd != &m_sldMic && pWnd != &m_antena)
    if ( pWnd != &m_antena)
	{
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}		
	// TODO: Return a different brush if the default is not desired

	return hbr;
}

void CUbiDialerDlg::OnBtnClose() 
{
	EnterCriticalSection(&endProcCritical);
	if( !bForeceEnd )
	{
	    if(MessageBox("Do you want to quit Dialer?","Dialer Close",MB_YESNO|MB_ICONQUESTION)!=IDYES)
	    {
			LeaveCriticalSection(&endProcCritical);
		    return;
	    }
	}
#ifdef SIP_UA
	if( m_pPhone->CallInfo.eCallState == e_call_success )
	{
	    OnBtnEnd();
		m_pPhone->bRun = 0;
        if( !bForeceEnd )
		    Sleep(3000);
		else
			Sleep(400);
	}
#endif
	//m_pPhone->send_register(0);
	if( !bForeceEnd )
	{
	    KillTimer(1);
	    stop_ua(0);
	}
	else
	{
		stop_ua(1);
	}
	ReleaseUSBDeviceDll();
    if( !bForeceEnd )
	{
		Sleep(1000);
		bForeceEnd = FALSE;
		LeaveCriticalSection(&endProcCritical);
		CDialog::OnOK();
		return;
	}
	//bForeceEnd = 1;
    LeaveCriticalSection(&endProcCritical);
	exit(0);
}


void CUbiDialerDlg::OnMidLabel() 
{
	// TODO: Add your control notification handler code here
	
}

void CUbiDialerDlg::OnBtn0() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "0" );
#ifdef SIP_UA
	m_pPhone->PSetNum('0');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('0');
	}
	else
	{
		//m_pPhone->USBwave->playDTMF()
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "0");
	}
#endif
}

void CUbiDialerDlg::OnBtn1() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "1" );
#ifdef SIP_UA
	m_pPhone->PSetNum('1');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('1');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "1");
	}
#endif
}

void CUbiDialerDlg::OnBtn2() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "2" );
#ifdef SIP_UA
	m_pPhone->PSetNum('2');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('2');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "2");
	}
#endif
}

void CUbiDialerDlg::OnBtn3() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "3" );
#ifdef SIP_UA
	m_pPhone->PSetNum('3');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('3');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "3");
	}
#endif
}

void CUbiDialerDlg::OnBtn4() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "4" );
#ifdef SIP_UA
	m_pPhone->PSetNum('4');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('4');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "4");
	}
#endif
}

void CUbiDialerDlg::OnBtn5() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "5" );
#ifdef SIP_UA
	m_pPhone->PSetNum('5');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('5');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "5");
	}
#endif
}

void CUbiDialerDlg::OnBtn6() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "6" );
#ifdef SIP_UA
	m_pPhone->PSetNum('6');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('6');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "6");
	}
#endif
}

void CUbiDialerDlg::OnBtn7() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "7" );
#ifdef SIP_UA
	m_pPhone->PSetNum('7');
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('7');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "7");
	}
#endif
}

void CUbiDialerDlg::OnBtn8() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "8" );
#ifdef SIP_UA
	m_pPhone->PSetNum('8');
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('8');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "8");
	}
#endif
}

void CUbiDialerDlg::OnBtn9() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "9" );
#ifdef SIP_UA
	m_pPhone->PSetNum('9');
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('9');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "9");
	}
#endif
}

void CUbiDialerDlg::OnBtnSt() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "*" );
#ifdef SIP_UA
	m_pPhone->PSetNum('*');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('*');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "*");
	}
#endif
}

void CUbiDialerDlg::OnBtnSh() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	m_lblMid.AddString( "#" );
#ifdef SIP_UA
	m_pPhone->PSetNum('#');	
	if( m_pPhone->CallInfo.eCallState != e_call_ready &&
		m_pPhone->CallInfo.eCallState != e_call_end)
	{
		m_pPhone->SendDtmf('#');
	}
#else
	if( strlen( this->cH323CallInfo.m_sOtherE164 ) < 64 )
	{
		strcat(this->cH323CallInfo.m_sOtherE164, "#");
	}
#endif
}

void CUbiDialerDlg::GetNowTime()
{
	time_t	ct;
	struct tm tm1;
	char 	szTmp[10], szBuf[32];
	CString	strTmp;

	time(&ct);
	tm1 = *localtime(&ct);
#ifdef SIP_UA
	if( m_pPhone->CallInfo.eCallState == e_call_success )
	{
		int h,m,s;
		ct2 = ct - m_pPhone->call_start;
		h = ct2/3600;
		m = (ct2- h*3600)/60;
		s = ct2- h*3600 - m*60;
		sprintf( szBuf, "%02d:%02d:%02d", h,m,s);
		m_lblMsg.SetWindowText(szBuf);
		m_lblMsg.RedrawWindow();
	}
	if( tm1.tm_hour > 12 )
	{
		tm1.tm_hour -= 12;
		strcpy( szTmp, "P" );
	}
	else
		strcpy( szTmp, "A" );
#endif
	sprintf( szBuf, "%s%2.2d:%2.2d", szTmp, tm1.tm_hour, tm1.tm_min );
	m_lblTime.SetLabel( szBuf );
}

void CUbiDialerDlg::OnBtnEnd() 
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
	KillTimer(10);
	KillTimer(20);
#ifdef SIP_UA
	if( m_pPhone->CallInfo.eCallState == e_call_end ||
		m_pPhone->CallInfo.eCallState == e_call_ready )
	{
        OnDisConnected();
	    hid_buzz_off();
		KillTimer(50);
        busycount = 0;
		m_pPhone->busystart = 0;
		return ;
	}
	if( !m_pPhone->CallInfo.m_call_transfer)
	{
		if( m_pPhone->CallInfo.eCallState == e_call_success )
		{
			bplay1 = 0;
	        ret = m_pPhone->send_bye();
			bCalling = 0;
	        m_lblMsg.SetWindowText("Wait for bye's answer.");
		}
		else
		{
			if( bCalling )
			{
			    bplay1 = 0;
                ret = m_pPhone->send_cancel();
				bCalling = 0;
	            m_lblMsg.SetWindowText("Wait for cancel's answer.");
			}
		}
	}
	else
	{
		ret = m_pPhone->send_refer();
	    m_lblMsg.SetWindowText("Wait for refer's answer.");
	}
#elif defined H323_UA
	if( this->cH323CallInfo.bCalling || this->cH323CallInfo.bCalled )
	{
		DropCall(&this->cH323CallInfo,cmReasonTypeUndefinedReason);
	}
	else
	{
	}
	OnDisConnected();
	hid_buzz_off();
	KillTimer(50);
	m_lblMsg.SetWindowText("");
	memset(this->cH323CallInfo.m_sOtherE164, 0, 64);
	//EndOfOperate();
#endif
	m_lblMsg.RedrawWindow();
}

void CUbiDialerDlg::OnBtnClear() 
{
	m_lblMid.DelChar();
#ifdef SIP_UA
	if( m_pPhone )
	{
		int len = strlen(m_pPhone->CallInfo.target_number);
		m_pPhone->CallInfo.target_number[len-1] = 0;
	}
#endif
}

void CUbiDialerDlg::OnBtnMini() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_MINIMIZE);
}

void CUbiDialerDlg::OnMicImage() 
{
	// TODO: Add your control notification handler code here
	CString	path;
	static	int	flag = 1;

	if( flag == 0 )
	{
		path.Format( "%s\\01_09_01_mi_on", MAIN_VOLUME_DIR );	
		flag = 1;
	}
	else
	{
		path.Format( "%s\\01_09_01_mi_off", MAIN_VOLUME_DIR );	
		flag = 0;
	}
	InitSkinBtn( m_imgMic, 8, 451, 22, 23, path );
}

void CUbiDialerDlg::OnSpkImage() 
{
	CString	path;
	static	int	flag = 1;

	if ( flag == 0 )
	{
		path.Format( "%s\\01_09_02_sp_on", MAIN_VOLUME_DIR );
		flag = 1;
	}
	else
	{
		path.Format( "%s\\01_09_02_sp_off", MAIN_VOLUME_DIR );
		flag = 0;
	}

	InitSkinBtn( m_imgSpk, 220, 451, 22, 23, path );				
}

void CUbiDialerDlg::OnBtnBook() 
{
	CPhoneBookDlg	dlg;

	if ( dlg.DoModal() == IDOK )
	{
		CButton *pBtnWnd = (CButton*)GetDlgItem(IDC_MID_LABEL);
		pBtnWnd->SetWindowText( m_szPhone );
#ifdef SIP_UA
		if( m_pPhone )
		{
			sprintf(m_pPhone->CallInfo.target_number, "%s", m_szPhone);
			deleteLabel(&m_lblMid);
		    m_lblMid.SetLabel( m_szPhone );
		    OnBtnSend();
		}
#else
			deleteLabel(&m_lblMid);
		    m_lblMid.SetLabel( m_szPhone );
		    OnBtnSend();
#endif
	}
}

void CUbiDialerDlg::OnBtnTelHst() 
{
	// TODO: Add your control notification handler code here
	CRecentTalkDlg	dlg;

	if ( dlg.DoModal() == IDOK )
	{
		CButton *pBtnWnd = (CButton*)GetDlgItem(IDC_MID_LABEL);
#if SIP_UA
		if( m_pPhone )
		{
		    pBtnWnd->SetWindowText( m_szPhone );
		    sprintf(m_pPhone->CallInfo.target_number, "%s", m_szPhone);
            deleteLabel(&m_lblMid);
		    m_lblMid.SetLabel( m_szPhone );
		    OnBtnSend();
		}
#else
	    pBtnWnd->SetWindowText( m_szPhone );
        deleteLabel(&m_lblMid);
	    m_lblMid.SetLabel( m_szPhone );
	    OnBtnSend();
#endif
	}
}

void CUbiDialerDlg::findHidDevice()
{
  ULONG      i;
  GUID       hidGuid;
  DWORD      requiredLength;
  HANDLE     tempDevice;
  HDEVINFO   hardwareDeviceInfo;
  char tempDevicePath[1024];


//  HIDP_CAPS                         tempCaps; // The Capabilities of this hid device.
  HIDD_ATTRIBUTES                   tempAttributes;
  PHIDP_PREPARSED_DATA              tempPpd;
  SP_INTERFACE_DEVICE_DATA          deviceInfoData;
  PSP_INTERFACE_DEVICE_DETAIL_DATA  detailDeviceData = NULL;

  HidD_GetHidGuid (&hidGuid);  //obtain GUID for HID
  //obtain HW info, it will be needed for other functions
  hardwareDeviceInfo = SetupDiGetClassDevs ( &hidGuid,
                                              NULL, // Define no enumerator (global)
                                              NULL, // Define no
                                             (DIGCF_PRESENT | // Only Devices present
                                              DIGCF_DEVICEINTERFACE)); // Function class devices.
  i=0;
  deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);
  while(i<10){ //10 max num of USb device, it is taken frealy, can be increased
    if (SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
                                     0, // No care about specific PDOs
                                     &hidGuid,
                                     i,
                                     &deviceInfoData)) {
      //
      // allocate a function class device data structure to receive the
      // goods about this particular device. Firs find out the size, and then alocate and fill it.
      //
      SetupDiGetDeviceInterfaceDetail (
                hardwareDeviceInfo,
                &deviceInfoData,
                NULL, // probing so no output buffer yet
                0, // probing so output buffer length of zero
                &requiredLength,
                NULL); // not interested in the specific dev-node
			detailDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA )new char[requiredLength];
                
			if(detailDeviceData) {
        detailDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);
      }
      else {
				SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
        delete []detailDeviceData; 
        return;
      }

      if ( SetupDiGetDeviceInterfaceDetail (
                           hardwareDeviceInfo,
                           &deviceInfoData,
                           detailDeviceData,
                           requiredLength,
                           0,
                           NULL) == 0) {
				SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
        delete []detailDeviceData; 
        return;
      } //end if SetupDiGetDeviceInterfaceDetail 
      strcpy( tempDevicePath, detailDeviceData->DevicePath );
      tempDevice = CreateFile(tempDevicePath,
                              0,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,        // no SECURITY_ATTRIBUTES structure
                              OPEN_EXISTING, // No special create flags
                              0, 
                              NULL);       // No template file
      if (INVALID_HANDLE_VALUE == tempDevice) {
				SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
        delete []detailDeviceData; 
        return;
      }

    //
    //Get the device information and fill it in the strucuture
    //
      if(!HidD_GetPreparsedData (tempDevice, &tempPpd)) {
        CloseHandle(tempDevice);
        delete []detailDeviceData; 
				SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
        return;
      }
      if(HidD_GetAttributes (tempDevice, &tempAttributes) != FALSE) {
        if ( (tempAttributes.VendorID == 0x74d)&(tempAttributes.ProductID == 0x3556)) {
          strcpy( devicePath, tempDevicePath);
          //device found 
          i = 11; //set i to value lareger then max num of device to end the loop and clean the structures;
        }
      }
  HIDP_CAPS                         tempCaps; // The Capabilities of this hid device.
  if(HidP_GetCaps(tempPpd, &tempCaps) == HIDP_STATUS_SUCCESS ){
  }
      delete []detailDeviceData; 
      HidD_FreePreparsedData (tempPpd);
      CloseHandle(tempDevice);
      i++;
    }//end if SetupDiEnumDeviceInterfaces
    else {
      break;
    }
  } //end while (i<arraySize)
	SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
  return;
}

void CUbiDialerDlg::hid_buzz_off()
{
	// TODO: Add your control notification handler code here
	if(hidWriteDevice != INVALID_HANDLE_VALUE){
		CloseHandle(hidWriteDevice);
		hidWriteDevice = INVALID_HANDLE_VALUE;
		return;
	}
	return;	
}

void CUbiDialerDlg::hid_buzz_on()
{
	// TODO: Add your control notification handler code here
	memset(devicePath, 0x00, sizeof(devicePath));
	findHidDevice();
	if(devicePath[0] == 0x00)
	{
		//    logText("HID not found");
		//MessageBox("HID not found");
		return;
	}
	hidWriteDevice = CreateFile(devicePath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,        // no SECURITY_ATTRIBUTES structure
		OPEN_EXISTING, // No special create flags
		0, 
		NULL);       // No template file
	if(hidWriteDevice == INVALID_HANDLE_VALUE){
		//    logText("HID write device open failed");
		//MessageBox("HID write device open failed");
		return;
	}
	return;	
}

void CUbiDialerDlg::hid_buzz_play_off()
{
	// TODO: Add your control notification handler code here
	hidTelegram[0] = 0x00;
	hidTelegram[1] = 0x02;
	for(int i=2; i<17;i++)
	{
		hidTelegram[i] = 0x00;
	}
	if(!WriteFile(hidWriteDevice, hidTelegram, 17, &writeLen, NULL))
	{
		//err
		//MessageBox("buzzer off : write file err");
		return;
	}
}

void CUbiDialerDlg::hid_buzz_play_on()
{
	// TODO: Add your control notification handler code here
	writeLen = 0x00;
	hidTelegram[0] = 0x00;
	hidTelegram[1] = 0x01;
	for(int i=2; i<17;i++){
		hidTelegram[i] = 0x00; 
	}
	if(!WriteFile(hidWriteDevice, hidTelegram, 17, &writeLen, NULL)) {
		//err
		//MessageBox("buzzer on : write file err");
		return;
	}
}

int CUbiDialerDlg::get_used_num(char *u_num)
{
	if( iLastUsedNumber < 1 || iLastUsedNumber >= 10 )
		return -1;
	sprintf(u_num, "%s", m_szUsedNumber[iLastUsedNumber-1]);
	return 1;
}

void CUbiDialerDlg::append_used_num(char *u_num)
{
	if( iLastUsedNumber == 10)
	{
	    int i;
	    for( i = 0; i < 9 ; i++)
	    {
		    sprintf(m_szUsedNumber[i], "%s", m_szUsedNumber[i+1]);
	    }
		sprintf(m_szUsedNumber[9], "%s", u_num);
	}
	else
	{
		sprintf(m_szUsedNumber[iLastUsedNumber++], "%s", u_num);
	}
}

void CUbiDialerDlg::OnBtnSend() 
{
	KillTimer(10);
	KillTimer(20);
	KillTimer(50);
	if( !bCalling && !bCalled )
	{
		MakeCall(this, &cH323CallInfo, m_CallConf.m_sSelfE164);
	}
}

void CUbiDialerDlg::SaveRecentTalk( t_Talk *pTalk )
{
	FILE	*fp;
	fp = fopen( "RecentTalk.Dat","a");
	if(fp == NULL || pTalk == NULL )
		return;
	fprintf( fp, "%s;%s;%1d;%s;%1d;%s;\n", 
		pTalk->szCallTime,
		pTalk->szPerson,
		pTalk->PhoneKind,
		pTalk->szPhoneNo,
		pTalk->InOutFlag,
		pTalk->szDuration );			
	fclose( fp );
}

BOOL CUbiDialerDlg::PreTranslateMessage(MSG* pMsg) 
{
	int key_down = 0;
	if((GetFocus()!=GetDlgItem(IDC_MSG_LABEL))
		&&(GetFocus()!= GetDlgItem(IDC_MID_LABEL)))
	{
		if( pMsg->wParam == 49 )
			printf("hello..\n");
		if( !bFirst )
		{
			if( pMsg->message == WM_KEYUP )
			{
				if( pMsg->wParam == VK_SHIFT )
				{
				    shift_down = 1;
				}
				else
				{
				    key_down = 1;
			        bFirst = TRUE;
				}
			}
			if( pMsg->message == WM_KEYDOWN )
			{
				//if( pMsg->wParam == VK_SHIFT )
				//{
				//  shift_down = 1;
				//}
				//key_down = 1;
			    bFirst = TRUE;
			}
		}
		if(pMsg->message == WM_KEYDOWN)
		{
			key_down = 1;
		}
		if( key_down == 1 )
		{
#ifdef SIP_UA
			if( !m_pPhone->bRegistered )
				return TRUE;
#elif defined H323_UA
			if(!m_bRegisteredBISCon)
				return TRUE;
#endif
			// if message is key down
			switch (pMsg->wParam)
			{
			case VK_UP:
				{
					int i=0;
                    if( iCurUsedNumber == -2 )
						iCurUsedNumber = iLastUsedNumber-1;
					else
					{
                        //iCurUsedNumber++;
						if( iCurUsedNumber > 9)
                            iCurUsedNumber = 0;
					}
					while(!strlen(m_szUsedNumber[iCurUsedNumber]))
					{
						if( i++ < 20 )
						{
						    if(iCurUsedNumber >= 9 )
						        iCurUsedNumber = 0;
							else if( iCurUsedNumber <= 0 )
						        iCurUsedNumber = 9;
							else
								iCurUsedNumber ++;
						}
						else
							break;
					}
					if( strlen(m_szUsedNumber[iCurUsedNumber]))
					{
#ifdef SIP_UA
				        sprintf(m_pPhone->CallInfo.target_number,"%s",m_szUsedNumber[iCurUsedNumber]);
#endif
				        deleteLabel(&m_lblMid);
#ifdef SIP_UA
				        m_lblMid.AddString(m_pPhone->CallInfo.target_number);
#endif
						iCurUsedNumber++;
					}
				}
				break;
			case VK_DOWN:
				{
					int i=0;
                    if( iCurUsedNumber == -2 )
						iCurUsedNumber = iLastUsedNumber-1;
					else
					{
                        //iCurUsedNumber--;
						if( iCurUsedNumber <0 )
						    iCurUsedNumber = 9;
					}
					while(!strlen(m_szUsedNumber[iCurUsedNumber]))
					{
						if( i++ < 20 )
						{
						    if( iCurUsedNumber <=0 )
						        iCurUsedNumber = 9;
							else if( iCurUsedNumber >= 9)
                                iCurUsedNumber = 0;
							else
								iCurUsedNumber --;
						}
						else
							break;
					}
					if( strlen(m_szUsedNumber[iCurUsedNumber]))
					{
#ifdef SIP_UA
				        sprintf(m_pPhone->CallInfo.target_number,"%s",m_szUsedNumber[iCurUsedNumber]);
#endif
				        deleteLabel(&m_lblMid);
#ifdef SIP_UA
				        m_lblMid.AddString(m_pPhone->CallInfo.target_number);
#endif
						iCurUsedNumber--;
					}
				}
				break;
			case VK_SHIFT:
				shift_down = 1;
				break;
			case VK_ESCAPE:
				OnBtnEnd();
				return TRUE;
			case VK_RETURN: // if enter			
#ifdef SIP_UA
				if( m_pPhone->CallInfo.eCallState == e_call_end ||
					m_pPhone->CallInfo.eCallState == e_call_ready ||
					m_pPhone->CallInfo.eCallState == e_call_called)
				{
				    OnBtnSend();
				}
				else if( m_pPhone->CallInfo.eCallState == e_call_success ||
					m_pPhone->CallInfo.eCallState == e_call_calling)
				{
					OnBtnEnd();
				}
#endif
				return TRUE;
				break;
			case VK_BACK:
				OnBtnClear();
				break;
			case 96:		
			case 48:
				OnBtn0();
				break;
			case 97:
			case 49:
				OnBtn1();
				break;
			case 98:
			case 50:
				OnBtn2();
				break;
			case 99:
			case 51:
				if( shift_down ) OnBtnSh();
				else OnBtn3();
				break;
			case 100:	
			case 52:
				OnBtn4();
				break;
			case 101:	
			case 53:
				OnBtn5();
				break;
			case 102:	
			case 54:
				OnBtn6();
				break;
			case 103:
			case 55:
				OnBtn7();
				break;
			case 106:
				OnBtnSt();
				break;
			case 104:
			case 56:
				if( shift_down ) OnBtnSt();
				else OnBtn8();
				break;
			case 105:
			case 57:
				OnBtn9();
				break;
			}
		}
		if(pMsg->message == WM_KEYUP)
		{
			switch (pMsg->wParam)
			{
			case (VK_SHIFT):
				shift_down = 0;
				break;
			}
		}
    }
	else 
	{
		if ((pMsg->message == WM_KEYDOWN && GetFocus())
		    != (GetDlgItem(IDC_MSG_LABEL) && pMsg->wParam == VK_RETURN))
	    {
		    pMsg->wParam = VK_TAB;
	    }
		else if( pMsg->message == WM_KEYDOWN )
		{
			if( pMsg->wParam == VK_RETURN )
			{
				return TRUE;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CUbiDialerDlg::SpekerVolControl(int value) 
{
	HMIXER	hMixer1;
	dwValue_head = value;

	if(dwValue_head > 65400) dwValue_head = 65400;

	if( mixerOpen(&hMixer1, m_nMixerID, 0, 0, 0) != MMSYSERR_NOERROR )	return; 
	

	// 볼륨 컨트롤에 대한 핸들을 얻는다..
	ZeroMemory(&mxc1, sizeof(MIXERCONTROL)); 
	if( GetMixerControl(hMixer1, 
		MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,//MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, 
		NO_SOURCE,		
		MIXERCONTROL_CONTROLTYPE_VOLUME,
		&mxc1) ) 
		{ 
			SetMixerValue(hMixer1, &mxc1, &dwValue_head);
		} 

	mixerClose(hMixer1);
}

void CUbiDialerDlg::MicVolControl(int value) 
{
	dwValue_head = value;

	if(dwValue_head > 65400) dwValue_head = 65400;

	HMIXER hmx; 
	mixerOpen(&hmx, 0, 0, 0, 0); 

	// Get the line info for the wave in destination line 
	MIXERLINE mxl; 
	mxl.cbStruct = sizeof(mxl); 
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 
	mixerGetLineInfo((HMIXEROBJ)hmx, &mxl, 
	MIXER_GETLINEINFOF_COMPONENTTYPE); 


	// Now find the microphone source line connected to this wave in 
	// destination 
	DWORD cConnections = mxl.cConnections; 
	for(DWORD j=0; j<cConnections; j++){ 
		mxl.dwSource = j; 
		mixerGetLineInfo((HMIXEROBJ)hmx, &mxl, MIXER_GETLINEINFOF_SOURCE); 
		if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) 
			break; 
	} 
	// Find a volume control, if any, of the microphone line 
	LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(sizeof MIXERCONTROL); 
	MIXERLINECONTROLS mxlctrl = {sizeof mxlctrl, mxl.dwLineID, 
	MIXERCONTROL_CONTROLTYPE_VOLUME, 1, sizeof MIXERCONTROL, pmxctrl}; 
	if(!mixerGetLineControls((HMIXEROBJ) hmx, &mxlctrl, 
		MIXER_GETLINECONTROLSF_ONEBYTYPE))	{ 
		// Found! 
		DWORD cChannels = mxl.cChannels; 
		if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl) 
			cChannels = 1; 

		LPMIXERCONTROLDETAILS_UNSIGNED pUnsigned = 
			(LPMIXERCONTROLDETAILS_UNSIGNED)malloc(cChannels * sizeof MIXERCONTROLDETAILS_UNSIGNED); 
		MIXERCONTROLDETAILS mxcd = {sizeof(mxcd), pmxctrl->dwControlID, 
			cChannels, (HWND)0, sizeof MIXERCONTROLDETAILS_UNSIGNED, (LPVOID) pUnsigned}; 
		mixerGetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE); 
		// Set the volume to the middle  (for both channels as needed) 
		pUnsigned[0].dwValue = pUnsigned[cChannels - 1].dwValue = dwValue_head;
			//(pmxctrl->Bounds.dwMinimum+pmxctrl->Bounds.dwMaximum)/2; 
		mixerSetControlDetails((HMIXEROBJ)hmx, &mxcd, 
			MIXER_SETCONTROLDETAILSF_VALUE); 

		free(pmxctrl); 
		free(pUnsigned); 
	} 
	else free(pmxctrl); 

	mixerClose(hmx); 

}

int CUbiDialerDlg::GetMixerControl(HMIXER hMixer, DWORD dwDstType, DWORD dwSrcType,DWORD dwCtrlType, MIXERCONTROL* pmxc) 
{ 
	//MIXERCONTROL mxc; // Holds the mixer control data. 
	MIXERLINE mxl; // Holds the mixer line data. 
	MIXERLINECONTROLS mxlc; // Obtains the mixer control. 
	// Initialize MIXERLINE structure.
	ZeroMemory(&mxl, sizeof(MIXERLINE));
	mxl.cbStruct = sizeof(MIXERLINE);
	GetDestLineIndex(hMixer, mxl, dwDstType);

	if( dwSrcType != NO_SOURCE )
		GetSourceLineIndex(hMixer, mxl, dwSrcType);
	if ( dwSrcType == -1 )
		GetSourceLineInfo(hMixer, mxl, 0);

	// Get the control.
	ZeroMemory(&mxlc, sizeof(MIXERLINECONTROLS));
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = dwCtrlType;
	mxlc.cControls = 1; 
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = pmxc; //&mxc; 
	pmxc->cbStruct = sizeof(MIXERCONTROL); 
	if( mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, 
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR ) { 
		//AfxMessageBox("Couldn't get the control."); 
		return 0; 
	} 
	return 1; 

} 

int CUbiDialerDlg::GetDestLineIndex(HMIXER hMixer, MIXERLINE &line, DWORD lineType)
{
	MIXERCAPS caps; 
	if( mixerGetDevCaps((UINT)hMixer, &caps, sizeof(MIXERCAPS)) != MMSYSERR_NOERROR ) 
	{ 
		return -1; 
	} 
	MIXERLINE	mxl;
	int nDest = caps.cDestinations; 
	for( int i = 0; i < nDest; i++ ) 
	{ 
		line.cbStruct = sizeof( MIXERLINE ); 
		mxl.cbStruct = sizeof( MIXERLINE ); 
		mxl.dwDestination = i;
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;//MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

		if( mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, 
			MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE/*MIXER_GETLINEINFOF_DESTINATION*/) != MMSYSERR_NOERROR ) 
		{ 
			return -1; 
		} 

		if( mxl.dwComponentType == lineType ) 
		{ 
			return( i ); 
		} 
	} 
	return -1; 
}

int CUbiDialerDlg::GetSourceLineIndex(HMIXER hMixer, MIXERLINE &line, DWORD lineType)
{
	UINT nConn = line.cConnections; 
 	DWORD dwDstIndex = line.dwDestination; 
 
 	for( UINT j = 0; j < nConn; j++ ) 
 	{ 
 		line.cbStruct = sizeof( MIXERLINE ); 
 		line.dwSource = j; 
 		line.dwDestination = dwDstIndex; 
 		if( mixerGetLineInfo((HMIXEROBJ)hMixer, &line, 
 			MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR ) 
	 	{ 
		 	return -1; 
		} 
 
 		if( line.dwComponentType == lineType ) 
 		{ 
 			return( j ); 
 		} 
	} 
 	return -1; 

}

int CUbiDialerDlg::GetSourceLineInfo(HMIXER hMixer, MIXERLINE &line, int index)
{
	UINT nConn = line.cConnections; 
 	DWORD dwDstIndex = line.dwDestination; 
 
 	line.cbStruct = sizeof( MIXERLINE ); 
 	line.dwSource = index; 
 	line.dwDestination = dwDstIndex; 
 	if( mixerGetLineInfo((HMIXEROBJ)hMixer, &line, 
 		MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR ) 
	{ 
		return -1; 
	} 
	return index;

}

int CUbiDialerDlg::SetMixerValue(HMIXER hMixer, MIXERCONTROL* pmxc, DWORD* pValue) 
{ 
	// After successfully getting the control, sets the control values. 
	MIXERCONTROLDETAILS mxcd; 
	//MIXERCONTROLDETAILS_UNSIGNED mxcdStruct={dwValue}; 
	
	DWORD dwControlID = pmxc->dwControlID; 
	DWORD cMultipleItems = pmxc->cMultipleItems; 
	// Initialize the MIXERCONTROLDETAILS structure 
	ZeroMemory(&mxcd, sizeof(MIXERCONTROLDETAILS)); 

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS); 
	mxcd.dwControlID = dwControlID; //mxc.dwControlID; 
	mxcd.cChannels = 1; 
	mxcd.cMultipleItems = cMultipleItems; 
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED); 
	mxcd.paDetails = pValue; 
	//mxcd.paDetails = &mxcdStruct; 

	// Set the values 
	if( mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, 
					MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR ) { 
		//AfxMessageBox("Couldn't set the control details."); 
		return 0; 
	} 
	return 1; 
} 

LONG CUbiDialerDlg::OnSlideChangeSpkVal(WPARAM wParam, LPARAM lParam)
{
	CSkinSlide	*pSlider = (CSkinSlide *)wParam;
	int		Val = (int)lParam * 654;
	char	Buff[100];

	sprintf( Buff, "%d", Val );	
	SpekerVolControl(Val) ;
	//AfxMessageBox( Buff );

	//m_lblTopMid.SetLabel( Buff );

	return 0;

}

LONG CUbiDialerDlg::OnSlideChangeMicVal(WPARAM wParam, LPARAM lParam)
{
	CSkinSlide	*pSlider = (CSkinSlide *)wParam;
	int		Val = (int)lParam * 654;
	char	Buff[100];

	sprintf( Buff, "%d", Val );	
	MicVolControl(Val) ;
    //AfxMessageBox( Buff );
	//m_lblTopMid.SetLabel( Buff );
	return 0;

}

LRESULT CUbiDialerDlg::OnRemainRecv(WPARAM wParam,LPARAM lParam)
{
	char	*p, szBuf[30];

	switch( wParam)
	{
	case 0:
		//m_pPhone->GetRemain( m_strAmount, m_strTime );
		m_lblAmount.SetLabel( m_strAmount );
		m_lblDur.SetLabel( m_strTime );
		break;
	case 1:
		//AfxMessageBox( "미등록");
		break;
	case 2:
		AfxMessageBox( "잔액부족");
		break;
	case 3:
		AfxMessageBox( "사용중");
		break;
	case 4:
		AfxMessageBox( "DB ERROR");
		break;	
	case 10:
		m_lblMid.SetLabel("");
		break;
	case 11:
		p = (char*)lParam;
		strcpy( szBuf, p );
		m_lblMid.SetLabel( szBuf );
		break;
	}
	return 0;
}

void CUbiDialerDlg::OnBtnSms() 
{
    // TODO: Add your control notification handler code here
    CSmsMainDlg dlg;
    dlg.DoModal();
}

void CUbiDialerDlg::OnBnClickedLabelAmount()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CUbiDialerDlg::OnBnClickedLabelDuration()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CUbiDialerDlg::OnBnClickedBtnConf()
{
//	CUBIConfDlg dlg;
	CUBIConf	dlg;
	if( !bCalled && !bCalling )
	{
		if( dlg.DoModal() == IDOK )
		{
#ifdef SIP_UA

#ifdef SIP_UA
			if( m_pPhone )
#endif
			{
				KillTimer(20000);
				KillTimer(300);
				//m_pPhone->send_register(0);
				//Sleep(100);
#ifdef SIP_UA
				m_pPhone->end_manager(1);
#endif
				//m_pPhone->rtpEnable = 0;
				//m_pPhone->OnRtpCloseChannel(m_pPhone->CallInfo.used_codec);
				//m_pPhone->OnRtpInClose();
				//m_pPhone->OnRtpOutClose();
				OnDisConnected();
				Sleep(200);
#ifdef SIP_UA
				m_pPhone->init_manager(1);
				m_pPhone->LoadConf();
#endif
				deleteLabel(&m_lblCallerNo);
				m_lblMsg.SetWindowText("Re-registering");
				m_lblMsg.RedrawWindow();
#ifdef SIP_UA
				if( !m_pPhone->start_manager(1) )
				{
                    if( sip_error_code ==1001)
					{
						AfxMessageBox("Fail to restart.[stun error:not supported nat]");
					}
					else
					{
					    AfxMessageBox("Fail to restart.");
					}
					return ;
				}
#endif
				SetTimer(1020, 20000, NULL);
				SetTimer(9, 300, NULL);
			}
#elif defined H323_UA
			AfxMessageBox("Restart this UA.");
#endif
		}
	}
}

void CUbiDialerDlg::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == NOWTIME_TIME)
	{
		KillTimer(nIDEvent);
		GetNowTime();
		SetTimer(nIDEvent, 1200,NULL);
	}
    else if( nIDEvent == 1020)
	{
        KillTimer(nIDEvent);
		if(!check_time_lock())
		{
			exit(0);
		}
		SetTimer(nIDEvent, 1000, NULL);
	}
	else if( nIDEvent == CHK_USB_SND)
	{
		KillTimer(CHK_USB_SND);
		int ret = MMSYSERR_NOERROR;
#ifdef SIP_UA
		ret = waveOutGetVolume(m_pPhone->USBOutwave->hwaveOut, (LPDWORD)&val);
#endif
		if( ret != MMSYSERR_NOERROR )
		{
#ifdef SIP_UA
			if( m_pPhone->bRun)
			{
				SendMessage(WM_USB_CLOSE_MSG);
			}
#endif
		}
		else
			SetTimer(CHK_USB_SND, 25,NULL);
	}
    else if( nIDEvent == 1)
	{
		if( hUSBModule && !m_bConnect)
		{
			DWORD dwErrorNo;
			// Detect USB Phone connect or not again and again
			if( UsbDeviceIOCTL(YL_IOCTL_OPEN_DEVICE, YLUSBTELCallBack, (DWORD)this, NULL, 0, &dwErrorNo) )
			{
				m_bConnect = TRUE;               // USB Phone have been connected now!
				UsbDeviceIOCTL(YL_IOCTL_GEN_UNREADY, NULL, 0, NULL, 0, NULL);
				//m_Status = STATUS_UNREADY;
				//SetDlgItemText(IDC_STATUS, "UnReady!");
			}
		}
	}
#ifdef SIP_UA
	else if( m_pPhone )
	{
        m_pPhone->OnTimer(nIDEvent);
	}
#endif
	CDialog::OnTimer(nIDEvent);
}

int CUbiDialerDlg::start_ua(void)
{
#ifdef SIP_UA
    if( !m_pPhone )
#endif
    {
#ifdef SIP_UA
        m_pPhone = new CSIPMain();
        if( m_pPhone )
#elif defined H323_UA
		if( ParseConfigure(this))
#endif
        {
            int i=0;
#ifdef SIP_UA
			if( !m_pPhone->voice_start(m_hWnd) )
			{
				AfxMessageBox("Not Supported Audio Device.");
				delete m_pPhone;
				return 0;
			}
			m_pPhone->rtpEnable = 0;
			m_pPhone->LoadConf();
#endif
			SetTimer(9, 20, NULL);
#ifdef SIP_UA
            if( m_pPhone->start_manager(0) )
			{
				SetTimer(1020, 20000, NULL);
				return 1;
			}
			else
			{
                if( sip_error_code ==1001)
				{
					AfxMessageBox("Fail to start.[stun error:not supported nat]");
					m_pPhone->stun_enable = 0;
					if( m_pPhone->start_manager(0) )
					{
						SetTimer(1020, 20000, NULL);
						return 1;
					}
				}
				else
				{
					AfxMessageBox("Fail to start.");
				}
				return 0;
			}
#elif defined H323_UA
			if( hApp)
			{
				if( waveCtl.VoiceStart(m_CallConf.m_InitOfRtpPort) )
				{
					cmStart(hApp);
					cmRegister(hApp);
				}
				else
				{
					DestroyWindow();
					return 0;
				}
				return 1;
			}
			else
			{
				return 0;
			}
#endif
        }
    }
	AfxMessageBox("Fail to start.");
	return 0;
}

void CUbiDialerDlg::stop_ua(int opt)
{
#ifdef SIP_UA
    if( m_pPhone )
#endif
    {
#ifdef SIP_UA
		if(! m_pPhone->bRun)
			return;
#endif
#ifdef SIP_UA
		if( !opt )
		    m_pPhone->voice_stop();
        m_pPhone->end_manager(0);
        delete m_pPhone;
#endif
    }
	KillTimer(1020);
}

LRESULT CUbiDialerDlg::OnWmUserMsg(WPARAM wParam, LPARAM lParam)
{
	switch((int)wParam)
	{
#ifdef SIP_UA
	case WM_SIPMSG_SD_INV:
		m_lblMsg.SetWindowText("calling");
		m_lblMsg.RedrawWindow();
		break;
	case WM_SIPMSG_SD_INV_SUCC:
		KillTimer(20);
		deleteLabel(&m_lblMsg);
		m_lblMsg.SetWindowText("Call Success");
		m_lblMsg.RedrawWindow();
        OnConnected(1);
		Start_Talk(1);
		if( m_pPhone )
		{
			time(&m_pPhone->call_start);
			m_pPhone->tm2 = *localtime(&m_pPhone->call_start);
		}
		break;
	case WM_SIPMSG_RV_INV:
		{
			KillTimer(10);
			KillTimer(20);
			KillTimer(50);
		    char *uNum=(char*)lParam;
		    bCalled = 1;
			m_lblMsg.SetWindowText("Called");
			m_lblMsg.RedrawWindow();
		    if( uNum )
			{
			    deleteLabel(&m_lblMid);
	            m_lblMid.AddString(uNum);
			    free(uNum);
            }
            SetTimer(10, 30, NULL);
			hid_buzz_play_on();
		}
		break;
	case WM_SIPMSG_SD_NEW_CALL_OK:
		if( m_pPhone )
		{
			time(&m_pPhone->call_start);
			m_pPhone->tm2 = *localtime(&m_pPhone->call_start);
		}
		Start_Talk(0);
		break;
	case WM_SIPMSG_RV_INV_SUCC:
		KillTimer(10);
		m_lblMsg.SetWindowText("Call Success");
		m_lblMsg.RedrawWindow();
		OnConnected(0);
		if( m_pPhone )
		{
			time(&m_pPhone->call_start);
			m_pPhone->tm2 = *localtime(&m_pPhone->call_start);
		}
		Start_Talk(0);
		//if( !m_pPhone->CallInfo.m_call_transfer )
		//    hid_buzz_play_off();
		break;
	case WM_SIPMSG_RV_BYE: // recv bye.
		KillTimer(10);
		KillTimer(20);
		m_lblMsg.SetWindowText("Disconnect");
		m_lblMsg.RedrawWindow();
		End_Talk(1);
		OnDisConnected();
		break;
	case WM_SIPMSG_SD_BYE_SUCC: // recv bye ok.
		//KillTimer(10);
		//KillTimer(20);
    	m_lblMsg.SetWindowText("Disconnect");
		m_lblMsg.RedrawWindow();
		End_Talk(1);
		OnDisConnected();
		break;
	case WM_SIPMSG_UNSUPP_METHOD:
		{
			//m_lblMsg.SetWindowText("Unsupported Method[405]");
			//m_lblMsg.RedrawWindow();
			break;
		}
	case WM_SIPMSG_SD_INV_FAIL:
		{
			char tbuf[128];
			KillTimer(10);
			KillTimer(20);
            int error_code = (int)lParam;
			switch( error_code )
			{
			case 484:
			case 404: // not found
				sprintf(tbuf,"Not Found[%d]", error_code);
				// play dial tone
				break;
			case 408: // time out
				sprintf(tbuf,"Request Time Out[%d]", error_code);
				break;
			case 400:
				sprintf(tbuf,"Unknow Error[%d]", error_code);
				break;
			case 486: // busy
				sprintf(tbuf,"Busy[%d]", error_code);
				break;
			default:
				sprintf(tbuf,"Unknow Error[%d]", error_code);
			}
			m_lblMsg.SetWindowText(tbuf);
			m_lblMsg.RedrawWindow();
			End_Talk(0);
			OnDisConnected(error_code);
		}
		break;
	case WM_SIPMSG_RV_INV_FAIL:
		KillTimer(10);
		KillTimer(20);
		hid_buzz_play_off();
		m_lblMsg.SetWindowText("Connection Fail");
		m_lblMsg.RedrawWindow();
		End_Talk(0);
		OnDisConnected();
		break;
	case WM_SIPMSG_SIP_INFO_RV:
		{
            if( (int)lParam )
		    {
				if( !chatdlg )
				{
					if( m_pPhone->CallInfo.eCallState == e_call_success)
	                    m_btnChat.SendMessage(WM_LBUTTONDOWN, 0, 0);
                        m_btnChat.SendMessage(WM_LBUTTONUP, 0, 0);
				}
		    }
			else
			{
				if( chatdlg )
				{
					sprintf(chat_msg,"%s", m_pPhone->sdp_msg);
					chatdlg->SendMessage(WM_CHAT_MSGRV, 0, 0);
				}
			}
		}
		break;
#endif
	case WM_UMSG_SD_REG_SUCC:
		{
			deleteLabel(&m_lblCallerNo);
			m_lblCallerNo.AddString(m_CallConf.m_sSelfE164);
#ifdef SIP_UA
			if( m_pPhone->CallInfo.eCallState == e_call_ready
				|| m_pPhone->CallInfo.eCallState == e_call_end)
#endif
			{
				m_lblMsg.SetWindowText("Ready");
				m_lblMsg.RedrawWindow();
			}
			memset(cH323CallInfo.m_sOtherE164, 0, 64);
			//deleteLabel(&m_antena);
			//m_antena.AddString("z");
			CtlUI(TRUE);
            ViewAntena();
		}
		break;
    case WM_UMSG_SD_REG_FAIL:
		{
		    deleteLabel(&m_lblCallerNo);
    		m_lblMsg.SetWindowText("unregistered");
	        m_lblMsg.RedrawWindow();
			memset(this->cH323CallInfo.m_sOtherE164, 0, 64);
			CtlUI(FALSE);
			//deleteLabel(&m_antena);
			//m_antena.AddString("u");
			ViewAntena();
		}
		break;
	case WM_UMSG_RV_INV_FAIL:
		KillTimer(10);
		KillTimer(20);
		hid_buzz_play_off();
		m_lblMsg.SetWindowText("Fail to Accept to Invite.");
		m_lblMsg.RedrawWindow();
		End_Talk(0);
		OnDisConnected();
		break;
	case WM_UMSG_SD_INV_FAIL:
		{
			char tbuf[128];
			KillTimer(10);
			KillTimer(20);
            int error_code = (int)lParam;
#ifdef SIP_UA
			switch( error_code )
			{
			case 484:
			case 404: // not found
				sprintf(tbuf,"Not Found[%d]", error_code);
				// play dial tone
				break;
			case 408: // time out
				sprintf(tbuf,"Request Time Out[%d]", error_code);
				break;
			case 400:
				sprintf(tbuf,"Unknow Error[%d]", error_code);
				break;
			case 486: // busy
				sprintf(tbuf,"Busy[%d]", error_code);
				break;
			default:
				sprintf(tbuf,"Unknow Error[%d]", error_code);
			}
#else
			wsprintf(tbuf, "Fail to invite. [%d]", error_code);
#endif
			m_lblMsg.SetWindowText(tbuf);
			m_lblMsg.RedrawWindow();
			End_Talk(0);
			OnDisConnected(error_code);
		}
		break;
	case WM_UMSG_RV_BYE: // recv bye.
		KillTimer(10);
		KillTimer(20);
		m_lblMsg.SetWindowText("Disconnect");
		m_lblMsg.RedrawWindow();
		End_Talk(1);
		OnDisConnected();
		break;
	case WM_UMSG_SD_BYE_SUCC: // recv bye ok.
		//KillTimer(10);
		//KillTimer(20);
    	m_lblMsg.SetWindowText("Disconnect");
		m_lblMsg.RedrawWindow();
		End_Talk(1);
		OnDisConnected();
		break;
	case WM_UMSG_SD_INV:
		m_lblMsg.SetWindowText("calling");
		m_lblMsg.RedrawWindow();
		this->cH323CallInfo.bCalling = 1;
		break;
	case WM_UMSG_SD_INV_SUCC:
		KillTimer(20);
		deleteLabel(&m_lblMsg);
		m_lblMsg.SetWindowText("Call Success");
		m_lblMsg.RedrawWindow();
        OnConnected(1);
		Start_Talk(1);
#ifdef SIP_UA
		if( m_pPhone )
		{
			time(&m_pPhone->call_start);
			m_pPhone->tm2 = *localtime(&m_pPhone->call_start);
		}
#endif
		break;
	case WM_UMSG_RV_INV:
		{
			this->cH323CallInfo.bCalled = 1;
			KillTimer(10);
			KillTimer(20);
			KillTimer(50);
		    char *uNum=(char*)lParam;
		    bCalled = 1;
			m_lblMsg.SetWindowText("Called");
			m_lblMsg.RedrawWindow();
		    if( uNum )
			{
			    deleteLabel(&m_lblMid);
	            m_lblMid.AddString(uNum);
			    free(uNum);
            }
            SetTimer(10, 30, NULL);
			hid_buzz_play_on();
		}
		break;
	case WM_UMSG_SD_NEW_CALL_OK:
#ifdef SIP_UA
		if( m_pPhone )
		{
			time(&m_pPhone->call_start);
			m_pPhone->tm2 = *localtime(&m_pPhone->call_start);
		}
#endif
		Start_Talk(0);
		break;
	case WM_UMSG_RV_INV_SUCC:
		KillTimer(10);
		m_lblMsg.SetWindowText("Call Success");
		m_lblMsg.RedrawWindow();
		OnConnected(0);
#ifdef SIP_UA
		if( m_pPhone )
		{
			time(&m_pPhone->call_start);
			m_pPhone->tm2 = *localtime(&m_pPhone->call_start);
		}
#else
#endif // SIP_UA
		Start_Talk(0);
		//if( !m_pPhone->CallInfo.m_call_transfer )
		//    hid_buzz_play_off();
		break;
	default:
		//KillTimer(10);
		//KillTimer(20);
		//KillTimer(50);
		break;
	}
	return NULL;
}

// USB 디바이이스의 DLL을 로드하여 핸들을 얻음
int CUbiDialerDlg::LoadUSBDeviceDll(void)
{
	hUSBModule = ::LoadLibrary("YLUSBTEL.dll");
	if( hUSBModule )
		return 1;
	hUSBModule = NULL;
	return 0;
}


// USB 디바이스를 해제하고, DLL의 핸들을 해제한다.
void CUbiDialerDlg::ReleaseUSBDeviceDll(void)
{
	if( hUSBModule )
	{
		DWORD dwErrorNo;
		if( UsbDeviceIOCTL(YL_IOCTL_CLOSE_DEVICE, NULL, 0, NULL, 0, &dwErrorNo))
		{
			m_bConnect = FALSE;               // USB Phone have been connected now!
			UsbDeviceIOCTL(YL_IOCTL_GEN_UNREADY, NULL, 0, NULL, 0, NULL);
			//m_Status = STATUS_UNREADY;
			//SetDlgItemText(IDC_STATUS, "UnReady!");
		}
		::FreeLibrary(hUSBModule);
		Sleep(100);
	}
	hUSBModule = NULL;
}

LRESULT CUbiDialerDlg::IOSendMessage(WPARAM wParam, LPARAM lParam)
{
	switch((DWORD)wParam)
	{
	case YL_CALLBACK_GEN_KEYDOWN: 	//button pressed
	    {
			// 0 = 128
			// 1 = 129
		    int key = (int)(DWORD)lParam;
#ifdef SIP_UA
			if( !m_pPhone )
				return NULL;
#endif
#ifdef SIP_UA
			if( m_pPhone->CallInfo.eCallState == e_call_success )
#else
			BOOL bConnect=TRUE;
			if(bConnect)
#endif
			{
			    switch(key)
			    {
			    case 128: // 0
				    OnBtn0();
				    break;
			    case 129: //1
				    OnBtn1();
				    break;
			    case 130: //2
				    OnBtn2();
				    break;
			    case 131: // 3
				    OnBtn3();
				    break;
			    case 132: // 4
				    OnBtn4();
				    break;
			    case 133: // 5
				    OnBtn5();
				    break;
			    case 134: // 6
				    OnBtn6();
				    break;
			    case 135: // 7
				    OnBtn7();
				    break;
			    case 136: // 8
				    OnBtn8();
				    break;
			    case 137: // 9
    				OnBtn9();
	    			break;
				}
			}
			else
			{
		    	if(key ==  145)// send
				{
			    	OnBtnSend();
					
    				//OnBtnClear();
				}
			}
	    }
	    break;
	case YL_CALLBACK_GEN_KEYBUF_CHANGED:
		{
			LPCSTR lpKeyBuf = (LPCSTR)lParam;
#ifdef SIP_UA
			if( m_pPhone )
#endif
			{
                deleteLabel(&m_lblMid);
				m_lblMid.AddString(lpKeyBuf);
#ifdef SIP_UA
				memset(m_pPhone->CallInfo.target_number, 0, 64);
				sprintf(m_pPhone->CallInfo.target_number, "%s", lpKeyBuf);
#endif
			}
		}
		break;
	case YL_CALLBACK_GEN_HANGUP:
		{
			OnBtnEnd();
		}
		break;
	default:
		break;
	}
	return 0;
}

void CUbiDialerDlg::OnBnClickedBtnRecharge()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	::ShellExecute( GetSafeHwnd(), "open", "http://www.anyuserusa.com", NULL, NULL, SW_SHOW );
}

void CUbiDialerDlg::deleteLabel(void * Item)
{
	int len,i;
	CSkinLabel * label = (CSkinLabel *)Item;
#ifdef SIP_UA
	if( m_pPhone )
#endif
	{
		CString str = label->GetLabel();
		len = str.GetLength();
	    for( i = 0; i< len ; i++)
	    	label->DelChar();
	}
}

void CUbiDialerDlg::OnClose()
{
	CDialog::OnClose();
}

int CUbiDialerDlg::UsbDeviceIOCTL(DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned)
{
	if( hUSBModule )
	{
		YLDEVIOCTRL *IOCTL;
		IOCTL = (YLDEVIOCTRL*)::GetProcAddress(hUSBModule,"YL_DeviceIoControl");
		if( IOCTL )
		{
			return IOCTL(dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);
		}
	}
    return 0;
}

void CUbiDialerDlg::End_Talk(int opt)
{
	if( opt )
	{
	    time_t end_time;
	    int sec,min,hour;
	    time(&end_time);
        end_time = end_time - call_start_time;
        
		hour = end_time/3600;
	    min = (end_time-hour*3600)/60;
	    sec = end_time - hour*3600 - min * 60;
	    sprintf(TalkBuff.szDuration,"%03d:%02d:%02d",hour,min,sec);
        SaveRecentTalk(&TalkBuff);
	}
	else
	{
	    memset( &TalkBuff, 0, sizeof( t_Talk ) );
	}
	::SendMessage(gMediaWnd, UM_CLOSEINDEV, 0, 0);
	::SendMessage(gMediaWnd, UM_CLOSEOUTDEV, 0, 0);
}

void CUbiDialerDlg::Start_Talk(int direction)
{
	CString m_Str,m_strBuf;
	struct  tm tm;
	char    szBuff[30];
	int i,len;
	char tbuf[128];

	UpdateData(TRUE);
    
	memset( &TalkBuff, 0, sizeof( t_Talk ) );
    
	_daylight = 0;
	time( &call_start_time );
	tm = *localtime( &call_start_time );
	sprintf( szBuff, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", 
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );
	strcpy( TalkBuff.szCallTime, szBuff );
	CButton *pBtnWnd = (CButton*)GetDlgItem(IDC_MID_LABEL);
	pBtnWnd->GetWindowText( m_Str );
	m_strBuf = m_Str.Left(3);
	if( m_strBuf == "011" || m_strBuf == "016" 
		|| m_strBuf == "018" || m_strBuf == "019" || m_strBuf == "010")
		TalkBuff.PhoneKind = 1;
	else
		TalkBuff.PhoneKind = 2;

	memset(tbuf, 0, 128);
#ifdef SIP_UA
	len = strlen(m_pPhone->CallInfo.target_number);
#else
	len = strlen(this->cH323CallInfo.m_sOtherE164);
#endif
	for( i = 0; i< len; i++)
	{
#ifdef SIP_UA
		if( m_pPhone->CallInfo.target_number[i] >= '0'
		    && m_pPhone->CallInfo.target_number[i] <= '9')
		{
			break;
		}
#else
		;
#endif
	}
#ifdef SIP_UA
	sprintf(tbuf,"%s", &m_pPhone->CallInfo.target_number[i]);
	strcpy( TalkBuff.szPerson, m_pPhone->CallInfo.target_display );
#endif
    strcpy( TalkBuff.szPhoneNo, tbuf);
	TalkBuff.InOutFlag = direction;
	//if( direction )
	::SendMessage(gMediaWnd, UM_OPENINDEV, this->cH323CallInfo.iChannelCodec+1, 0);
	::SendMessage(gMediaWnd, UM_OPENOUTDEV, this->cH323CallInfo.iChannelCodec+1, 0);
	::SendMessage(gMediaWnd, UM_REQOUTRTP, 0, 0);
}

void CUbiDialerDlg::OnConnected(int direction)
{
#ifdef SIP_UA
	if(! m_pPhone )
		return;
#endif
    if( direction )
	{
#ifdef SIP_UA
		if(m_bConnect)
			UsbDeviceIOCTL(YL_IOCTL_GEN_CALLOUT, (LPVOID)((LPCTSTR)m_pPhone->CallInfo.target_number), strlen(m_pPhone->CallInfo.target_number), NULL, 0, &dwErrorNo);
#else // SIP_UA
		if(m_bConnect)
			UsbDeviceIOCTL(YL_IOCTL_GEN_CALLOUT, (LPVOID)((LPCTSTR)cH323CallInfo.m_sOtherE164), strlen(cH323CallInfo.m_sOtherE164), NULL, 0, &dwErrorNo);
#endif
	    if(m_bConnect)
			UsbDeviceIOCTL(YL_IOCTL_GEN_RINGBACK, NULL, 0, NULL, 0, &dwErrorNo);
	}
	else
	{
#ifdef SIP_UA
        if(m_bConnect)
			UsbDeviceIOCTL(YL_IOCTL_GEN_CALLIN, (LPVOID)((LPCTSTR)m_pPhone->CallInfo.target_number),strlen(m_pPhone->CallInfo.target_number), NULL, 0,&dwErrorNo);
#endif
		//deleteLabel(&m_lblCallerNo);
	    //m_lblCallerNo.AddString(m_pPhone->CallInfo.target_number);
	}
}

void CUbiDialerDlg::OnDisConnected(int code/*=0*/)
{

#ifdef SIP_US
	if( code != 486 )
#endif
	{
	    bCalling = 0;
	    bCalled = 0;
	    deleteLabel(&m_lblMid);
#ifdef SIP_UA
		memset(m_pPhone->CallInfo.target_number, 0, 64);
	    m_lblMid.AddString(m_pPhone->CallInfo.target_number);
#else
		memset(cH323CallInfo.m_sOtherE164, 0, 64);
	    m_lblMid.AddString(cH323CallInfo.m_sOtherE164);
#endif
	    //deleteLabel(&m_lblCallerNo);
	}
#ifndef SIP_UA
	this->cH323CallInfo.bCalling = 0;
	this->cH323CallInfo.bCalled = 0;
#endif
    if(m_bConnect)
		UsbDeviceIOCTL(YL_IOCTL_GEN_READY, NULL, 0, NULL, 0, &dwErrorNo);
	Sleep(500);
#ifdef SIP_UA
	if( code != 486 )
#endif
	{
#ifdef SIP_UA
		if( m_pPhone->bRegistered )
	        m_lblMsg.SetWindowText("Ready");
	    else
	        m_lblMsg.SetWindowText("Unregisted");
#elif defined H323_UA
		if( m_bRegisteredBISCon )
	        m_lblMsg.SetWindowText("Ready");
	    else
	        m_lblMsg.SetWindowText("Unregisted");
#endif
	}
#ifdef SIP_UA
	else
	{
	    m_lblMsg.SetWindowText("Busy");
	}
#endif
	m_lblMsg.RedrawWindow();
}

void CUbiDialerDlg::CtlUI(BOOL opt)
{
	m_btnSend.EnableWindow(opt);
	m_btnClear.EnableWindow(opt);
	m_btnEnd.EnableWindow(opt);
	m_btn0.EnableWindow(opt);
	m_btn1.EnableWindow(opt);
	m_btn2.EnableWindow(opt);
	m_btn3.EnableWindow(opt);
	m_btn4.EnableWindow(opt);
	m_btn5.EnableWindow(opt);
	m_btn6.EnableWindow(opt);
	m_btn7.EnableWindow(opt);
	m_btn8.EnableWindow(opt);
	m_btn9.EnableWindow(opt);
	m_btnSharp.EnableWindow(opt);
	m_btnStar.EnableWindow(opt);
}

void CUbiDialerDlg::GetWaveOutVolume()
{
#if 0
	WAVEOUTCAPS	waveCaps;
	int iDev;
	
	if ( findAudioDevice(&waveCaps, &iDev) )
	{
		if(waveCaps.dwSupport & WAVECAPS_VOLUME)
		{
			DWORD volume;
			waveOutGetVolume((HWAVEOUT)iDev, (LPDWORD)&volume);

			DWORD left = LOWORD(volume);
			DWORD right = left*0x00010000UL;
			DWORD total = left + right;

			waveOutSetVolume((HWAVEOUT)iDev, total);

			int iVolume = (int)(left*0xFFFF/0xFFFF);

			m_sldSpk.SetPos(0xFFFF - iVolume);

			m_sldSpk.UpdateData(FALSE);
		}
	}
#endif
}

BOOL CUbiDialerDlg::findAudioDevice(LPWAVEOUTCAPS t_waveCaps, int *iDev)
{
    UINT uNumDevs;
    
	uNumDevs = waveOutGetNumDevs(); // audio device 의 수를 읽어옴. 
	int iDefault = -1;

	if ( uNumDevs == 0 ) // audio 장비 없음. 리턴.
		return FALSE;

	for ( UINT i=0; i<uNumDevs; i++ ) // default audio device 찾기 (USB Audio 장치)
		if (!waveOutGetDevCaps(i, t_waveCaps, sizeof(WAVEOUTCAPS)) )
			if ( !strcmp(t_waveCaps->szPname, DEFAULT_AUDIO_DEVICE) )
				iDefault = i;

	if ( iDefault != -1 )
	{	// 찾았음. 
		if ( waveOutGetDevCaps(iDefault, t_waveCaps, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR )
		{
			*iDev = iDefault;
			return TRUE;
		}
		else 
			return FALSE;
	}
	else 
	{	// 없음. 그냥 시스템 디폴트 장비를 가져옴. 
		if ( waveOutGetDevCaps(0, t_waveCaps, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
		{
			*iDev = 0;
			return TRUE;
		}
		else 
			return FALSE;
	}
}

void CUbiDialerDlg::SetWaveOutVolume(int iPos)
{
	int iVolume = 0xFFFF - iPos;
	WAVEOUTCAPS	waveCaps;

	int iDev;
	
	if ( findAudioDevice(&waveCaps, &iDev) )
	{
		if(waveCaps.dwSupport & WAVECAPS_VOLUME)
		{
			DWORD left = iVolume*0xFFFF/0xFFFF;
			DWORD right = left*0x00010000UL;
			DWORD total = left + right;

			waveOutSetVolume((HWAVEOUT)iDev, total);
		}
	}	
}

void CUbiDialerDlg::GetWaveInVolume()
{
#if 0
	MIXERCAPS	mixerCaps;
	HMIXER		hMixer;
	MIXERLINE	mxl; 
	int			iDev;

	if ( findAudioInDevice(&mixerCaps, &iDev) )
	{
		if ( mixerOpen(&hMixer, iDev, 0, 0L, CALLBACK_NULL) == MMSYSERR_NOERROR )
		{	// 열었다. 
			mxl.cbStruct = sizeof(MIXERLINE); 
			mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 
			mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE); 

			DWORD cConnections = mxl.cConnections; 
			for(DWORD j=0; j<cConnections; j++){ 
				mxl.dwSource = j; 
				mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE); 
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) 
					break; 
			} 

			LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(sizeof MIXERCONTROL); 
			
			MIXERLINECONTROLS mxlctrl = 
			{
				sizeof mxlctrl, mxl.dwLineID, 
				MIXERCONTROL_CONTROLTYPE_VOLUME, 1, 
				sizeof MIXERCONTROL, pmxctrl
			}; 

			if( mixerGetLineControls((HMIXEROBJ) hMixer, &mxlctrl, 
				MIXER_GETLINECONTROLSF_ONEBYTYPE ) == MMSYSERR_NOERROR )	
			{ // find.
				DWORD cChannels = mxl.cChannels; 
				if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl) 
					cChannels = 1; 

				LPMIXERCONTROLDETAILS_UNSIGNED pUnsigned = 
					(LPMIXERCONTROLDETAILS_UNSIGNED)malloc(cChannels * sizeof MIXERCONTROLDETAILS_UNSIGNED); 
				MIXERCONTROLDETAILS mxcd = {sizeof(mxcd), pmxctrl->dwControlID, 
					cChannels, (HWND)0, sizeof MIXERCONTROLDETAILS_UNSIGNED, (LPVOID) pUnsigned}; 
				mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE); 
				
				int iVolume = pUnsigned[0].dwValue = pUnsigned[cChannels - 1].dwValue;				
				m_sldMic.SetPos(0xFFFF - iVolume);
				m_sldMic.UpdateData(FALSE);
				
				mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, 
					MIXER_SETCONTROLDETAILSF_VALUE); 

				free(pmxctrl); 
				free(pUnsigned); 
			} 
			else free(pmxctrl); 
		}
		mixerClose(hMixer);
	}
#endif
}

void CUbiDialerDlg::SetWaveInVolume(int iPos)
{
#if 0
	MIXERCAPS	mixerCaps;
	HMIXER		hMixer;
	MIXERLINE	mxl; 
	int			iDev;

	int iVolume = 0xFFFF - iPos;

	if ( findAudioInDevice(&mixerCaps, &iDev) )
	{
		if ( mixerOpen(&hMixer, iDev, 0, 0L, CALLBACK_NULL) == MMSYSERR_NOERROR )
		{	// 열었다. 
			mxl.cbStruct = sizeof(MIXERLINE); 
			mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 
			mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE); 

			DWORD cConnections = mxl.cConnections; 
			for(DWORD j=0; j<cConnections; j++){ 
				mxl.dwSource = j; 
				mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE); 
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) 
					break; 
			} 

			LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(sizeof MIXERCONTROL); 
			
			MIXERLINECONTROLS mxlctrl = 
			{
				sizeof mxlctrl, mxl.dwLineID, 
				MIXERCONTROL_CONTROLTYPE_VOLUME, 1, 
				sizeof MIXERCONTROL, pmxctrl
			}; 

			if( mixerGetLineControls((HMIXEROBJ) hMixer, &mxlctrl, 
				MIXER_GETLINECONTROLSF_ONEBYTYPE ) == MMSYSERR_NOERROR )	
			{ // public soundcard find.
				DWORD cChannels = mxl.cChannels; 
				if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl) 
					cChannels = 1; 

				LPMIXERCONTROLDETAILS_UNSIGNED pUnsigned = 
					(LPMIXERCONTROLDETAILS_UNSIGNED)malloc(cChannels * sizeof MIXERCONTROLDETAILS_UNSIGNED); 
				MIXERCONTROLDETAILS mxcd = {sizeof(mxcd), pmxctrl->dwControlID, 
					cChannels, (HWND)0, sizeof MIXERCONTROLDETAILS_UNSIGNED, (LPVOID) pUnsigned}; 
				mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE); 
				// Set the volume to the middle  (for both channels as needed) 
				pUnsigned[0].dwValue = pUnsigned[cChannels - 1].dwValue = iVolume;
					//(pmxctrl->Bounds.dwMinimum+pmxctrl->Bounds.dwMaximum)/2; 
				mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, 
					MIXER_SETCONTROLDETAILSF_VALUE); 

				free(pmxctrl); 
				free(pUnsigned); 
			} 
			else 
			{	// USB Phone find
				free(pmxctrl); 

				CMyAudioMixer m_AudioMixer;
				m_AudioMixer.Open();
				DWORD dwVolume = 0xFFFF - iPos;
				m_AudioMixer.AdjustProperty(dwVolume, 1|0x10000);
				m_AudioMixer.Close();	
			}
		}
		mixerClose(hMixer);
	}
#endif
}

BOOL CUbiDialerDlg::findAudioInDevice(LPMIXERCAPS t_mixerCaps, int *iDev)
{
	UINT uNumDevs;
    
	uNumDevs = mixerGetNumDevs(); // mixer 의 수를 읽어옴.
	int iDefault = -1;

	if( uNumDevs == 0 ) // audio 장비 없음. 리턴.
		return FALSE;

	for( UINT i=0; i<uNumDevs; i++ ) // default audio device 찾기 (USB Audio 장치)
		if (!mixerGetDevCaps(i, t_mixerCaps, sizeof(MIXERCAPS)) )
			if ( !strcmp(t_mixerCaps->szPname, DEFAULT_AUDIO_DEVICE) )
				iDefault = i;

	if( iDefault != -1 )
	{
		// 찾았음. 
		if ( mixerGetDevCaps(iDefault, t_mixerCaps, sizeof(MIXERCAPS)) == MMSYSERR_NOERROR )
		{
			*iDev = iDefault;
			return TRUE;
		}
		else 
			return FALSE;
	}
	else 
	{
		// 없음. 그냥 시스템 디폴트 장비를 가져옴. 
		if ( mixerGetDevCaps(0, t_mixerCaps, sizeof(MIXERCAPS)) == MMSYSERR_NOERROR)
		{
			*iDev = 0;
			return TRUE;
		}
		else 
			return FALSE;
	}
}

void CUbiDialerDlg::MicMute() 
{
#if 0
	if ( !bMicMute )
	{
		bMicMute = TRUE;

		SetWaveInVolume(0xFFFF);
	}
	else 
	{
		bMicMute = FALSE;

		int iPos = m_sldMic.GetPos();
		SetWaveInVolume(iPos);
	}
#endif
}

void CUbiDialerDlg::SpeakerMute() 
{
#if 0
	if ( !bSpeakerMute )
	{
		bSpeakerMute = TRUE;

		SetWaveOutVolume(0xFFFF);
	}
	else 
	{
		bSpeakerMute = FALSE;

		int iPos = m_sldSpk.GetPos();
		SetWaveOutVolume(iPos);
	}
#endif
}


//
////////////////////////////////////////////////////////////////////////
// end of file.
//
//
#if 0
void CUbiDialerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( bSpeakerMute )
	{
		SetWaveOutVolume(0xFFFF);
	}
	else 
	{
		int iPos = m_sldSpk.GetPos();
		SetWaveOutVolume(iPos);
	}

	if ( bMicMute )
	{
		SetWaveInVolume(0xFFFF);
	}
	else 
	{
		int iPos = m_sldMic.GetPos();
		SetWaveInVolume(iPos);
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
#endif
void CUbiDialerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	//ViewAntena();
}

int CUbiDialerDlg::GetVersionInfo(CString &csBuf)
{
	HINSTANCE hInstVer = ::AfxGetResourceHandle();
	if( hInstVer )
	{
		HRSRC hVersion = FindResource( hInstVer, 
			 MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
		if (hVersion != NULL)
		{

			HGLOBAL hGlobal = LoadResource( hInstVer, hVersion );
			if( hGlobal )
			{
				LPVOID versionInfo  = LockResource(hGlobal);
				if (versionInfo != NULL)
				{
					CString csRet;
					DWORD vLen,langD;
					BOOL retVal;
					LPVOID retbuf=NULL;
					static char fileEntry[256];
					sprintf(fileEntry,"\\VarFileInfo\\Translation");
					retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
					if (retVal && vLen==4)
					{
						memcpy(&langD,retbuf,4);
						sprintf(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
							 (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24,
							 (langD & 0xff0000)>>16, "FileVersion");
					}
					else
						sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s", GetUserDefaultLangID(), "FileVersion");
					if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen))
						 csBuf = (char*)retbuf;
                    UnlockResource( hGlobal );
                    FreeResource( hGlobal );
			        return 1;
				}
                FreeResource( hGlobal );
			}
		}
	}
	return 0;
}

typedef struct _phone_info
{
	char company[16];
	char version[16];
	char u_num[64];
	char u_ip[16];
    SYSTEMTIME st;
}PHONEINFO,*PPHONEINFO;

int CUbiDialerDlg::CheckUpdate()
{
#ifdef SIP_UA
    CSkySock sock;
	PHONEINFO phoneInfo;
    if( m_pPhone )
	{
	    if( sock.create_tcp() )
	    {
	        //if( !sock.connect_tcp("www.skycom.ne.kr", 1024) )
			//if( !sock.connect_tcp("sipproxy.skycom.com", 80) )
			if( !sock.connect_tcp("mail.skycom.ne.kr", 80) )
		    {
				CString csVer;
				unsigned char outbuf[260];
				int len;
				memset(outbuf, 0, 260);
				csVer = "1, 0, 0, 0";
				GetVersionInfo(csVer);
				csVer.Replace(",",".");
				csVer.Remove(' ');
				CBlowFish fish((unsigned char*)("sky_sip_ua"), 10);
                sprintf(phoneInfo.company,"%s", "ubistar");
				GetLocalTime(&phoneInfo.st);
                sprintf(phoneInfo.u_ip, "%s", m_pPhone->hostip);
				sprintf(phoneInfo.u_num, "%s", m_pPhone->u_number);
				sprintf(phoneInfo.version, "%s", csVer);
                outbuf[0] = 'S';
				outbuf[1] = 'O';
				outbuf[2] = 'F';
				outbuf[3] = 'T';
                fish.Encrypt((unsigned char*)&phoneInfo, &outbuf[4], 128);
			    sock.send_tcp((char*)&outbuf[0], 132);
				memset(outbuf, 0, 256);
				len = 256;
				sock.recv_tcp((char*)&outbuf[0], &len, 5);
				sock.close_handle();
				if( len == 0 )
					return 3;
				if( !memcmp(outbuf, "OK", 2) )
				{
					return 1;
				}
				return 2;
			}
			sock.close_handle();
			return 3;
		}
		return 0;
	}
	return 0;
#else
	return 1;
#endif
}

UINT createChat(PVOID pArg)
{
    CUbiDialerDlg * pDlg = (CUbiDialerDlg *)pArg;
	chatdlg = new CSkyChat;
	chatdlg->DoModal();
	delete chatdlg;
	chatdlg = NULL;
	AfxEndThread(0);
	return 0;
}


void CUbiDialerDlg::OnBnClickedBtnChat()
{
#ifdef SIP_UA
	if( m_pPhone )
#endif
	{
#ifdef SIP_UA
		if( m_pPhone->CallInfo.eCallState == e_call_success )
#else
#endif
		{
	        if( !chatdlg )
	        {
	            chatdlg = new CSkyChat;
#ifdef SIP_UA
				sprintf(chatdlg->target_num, "%s", m_pPhone->CallInfo.target_number);
#endif
				chatdlg->Create(IDD_DLG_CHAT,this);
				chatdlg->ShowWindow(SW_SHOW);
			}
		}
	}
}

LRESULT CUbiDialerDlg::OnWmChatSend(WPARAM wParam, LPARAM lParam)
{
#ifdef CHAT_SUPPORT
#ifdef SIP_UA
	if( chatdlg && m_pPhone)
	{
        m_pPhone->send_sip_info(sipInfo_msg, 0, chatdlg->u_input_str);
	}
#endif
#else // CHAT_SUPPORT
	AfxMessageBox("Not Support. ( Supported only SIP Mode. )");
#endif // CHAT_SUPPORT
	return NULL;
}
#if 0
void CUbiDialerDlg::OnBnClickedBtnTransfer()
{
#ifdef SIP_UA
	if(m_pPhone)
#endif
	{
#ifdef SIP_UA
		if(m_pPhone->CallInfo.eCallState == e_call_success )
		{
			m_pPhone->set_transfer();
		    deleteLabel(&m_lblMid);
			//m_pPhone->send_transfer();
		}
		else
		{
			AfxMessageBox("Not connected.",MB_OK|MB_ICONINFORMATION);
		}
#endif
	}
}
#endif
LRESULT CUbiDialerDlg::OnWmDownUSB(WPARAM wParam,LPARAM lParam)
{
	//if( m_pPhone->CallInfo.eCallState == e_call_success)
	//{
	if( ! bForeceEnd )
	{
		bForeceEnd = TRUE;
		OnBtnClose();
	}
	return NULL;
}

LRESULT CUbiDialerDlg::OnChatDlgDown(WPARAM wParam,LPARAM lParam)
{
    delete chatdlg;
	chatdlg = NULL;
	return NULL;
}

/////////////////////////////////////////////////////
//
//



void CUbiDialerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
#ifndef SIP_UA
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,"WM_DESTROY");
	waveCtl.VoiceStop();
	
	if( hEvent )
		WaitForSingleObject(hEvent,300);
	else
		Sleep(100);
	
	if( hApp )
		cmStop(hApp);
	
	if( hEvent )
		WaitForSingleObject(hEvent,300);
	else
		Sleep(100);
	if( bCalling || bCalled)	
		EndOfOperate();
	
	if( hEvent )
	{
		WaitForSingleObject(hEvent,300);
		CloseHandle(hEvent);
	}
	else
		Sleep(100);
#endif
}
