// D:\Skycom_work\UBI_Dialer\src\SkyChat.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UbiDialer.h"
#include "UbiDialerDlg.h"
#include "SkyChat.h"
//#include "skychat.h"
#ifdef SIP_UA
#include "cProfile.h"
#endif
//#include "SymbolDlg.h"
#include "skychat.h"

CSkyChat *pChatDlg = NULL;
extern CUbiDialerDlg	*g_PhoneDlg;
#ifdef SIP_UA
#define SIPCALLER_CONF "./conf/sky_ua.cfg"
#endif
extern CSkyChat *chatdlg;

// CSkyChat 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkyChat, CDialog)
CSkyChat::CSkyChat(CWnd* pParent /*=NULL*/)
	: CDialog(CSkyChat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkyChat)
	m_strMsg = _T("");	
	//}}AFX_DATA_INIT
}

CSkyChat::~CSkyChat()
{
}

void CSkyChat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_EXIT, m_close);
	DDX_Control(pDX, IDC_BTN_MIN, m_min);
	DDX_Control(pDX, IDC_MESSAGE_WND, m_view_wnd);
	DDX_Control(pDX, IDC_EDT_TALK, m_talk);
	//DDX_Control(pDX, IDC_EDT_NUM, m_num);
	DDX_Control(pDX, IDC_CALLED_LABEL, m_lblCalledNo);
	DDX_Control(pDX, IDC_BTN_SEND, m_send);
	DDX_Control(pDX, IDC_LIST1, m_lstWnd);
	DDX_Control(pDX, IDC_BTN_SYMBOL1, m_symbol);
	DDX_Text(pDX, IDC_EDT_TALK, m_strMsg);
}

BEGIN_MESSAGE_MAP(CSkyChat, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_SEND, OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_SYMBOL1, OnBnClickedBtnSymbol1)
	ON_MESSAGE(WM_CHAT_MSGRV,OnMsgChatRcv)	
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CSkyChat 메시지 처리기입니다.
/////////////////////////////////////////////////////////////////////////////
// CSmsMainDlg message handlers
BOOL CSkyChat::OnEraseBkgnd(CDC* pDC) 
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
UINT CSkyChat::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	UINT    nReturn = CDialog::OnNcHitTest(point);

    if(nReturn == HTCLIENT)	nReturn = HTCAPTION;  

    return nReturn;  
}

HBRUSH CSkyChat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if ( pWnd != &m_talk && pWnd != &this->m_view_wnd && pWnd != &this->m_lstWnd)
	{
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CSkyChat::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	pChatDlg = this;
	// TODO: Add extra initialization here
	SetDlgItemText(IDC_EDT_TALK, "startCHAT");
	OnBnClickedBtnSend();
	init_skin();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSkyChat::init_skin()
{
	CString path;

	InitSkinHomeDir();
	InitSkinFont();
	if( !InitSkinBkGnd( "03_sub\\03_09_chatting\\03_09_00_bg.bmp" ) )	return;

//Send
	path.Format( "%s\\03_00_16_chat_send", SUB_BUTTON_DIR );
	InitSkinBtn( m_send, 305, 353, 42, 24, path );	//348, 379
//Close
	path.Format( "%s\\03_00_00_close", SUB_BUTTON_DIR );
	InitSkinBtn( m_close, 328, 10, 29, 28, path );
//minimize
	path.Format( "%s\\03_00_15_mini", SUB_BUTTON_DIR );
	InitSkinBtn( m_min, 296, 10, 29, 28, path );
//SMS Message
	m_talk.MoveWindow( 21, 355, 280, 24, TRUE );	
	m_talk.bkColor( RGB(199, 200, 207) );
	//m_num.MoveWindow( 90, 55, 201, 21, TRUE );
	//m_num.bkColor( RGB(239, 240, 243) );
	//m_view_wnd.MoveWindow( 14, 44, 341, 297, TRUE ); //355,44 355,341
	m_view_wnd.ShowWindow(SW_HIDE);
	m_lstWnd.MoveWindow( 14, 44, 341, 297, TRUE ); //355,44 355,341
//Symbol
	path.Format( "%s\\03_00_10_special", SUB_BUTTON_DIR );
	InitSkinBtn( m_symbol, 21, 393, 50, 30, path );
	
	InitSkinLabel( m_lblCalledNo, 160, 15, 100, 20, &m_CalledFont );
	m_lblCalledNo.AddString( target_num );

	UCHAR	tmp = DEFAULT_PITCH | FF_DONTCARE;
	m_lstWnd.setFont( 16, FW_NORMAL, tmp, "Arial" );
	m_lstWnd.bkColor( RGB(239, 240, 243) );	
	m_view_wnd.bkColor( RGB(239, 240, 243) );
}

void CSkyChat::InitSkinHomeDir()
{
	FILE	*fp;

	strcpy( m_SkinDir, ".\\bmp" );

	if( ( fp = fopen( "Skin.Dir", "r" ) ) == NULL )return;
	fscanf( fp, "%s", m_SkinDir );
	fclose( fp );
}

BOOL CSkyChat::InitSkinBkGnd( char *BkGndFile )
{
	char	FName[200];

	sprintf( FName, "%s\\%s", m_SkinDir, BkGndFile );
	if ( !LoadBitmapFile( m_bmpBkGnd, FName ) )	return FALSE;

	BITMAP bmp;
	m_bmpBkGnd.GetBitmap( &bmp );
	SetWindowPos( NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOZORDER );
	BitmapRgn( m_hWnd, (HBITMAP)m_bmpBkGnd, RGB( 255,0,255 ) );
	
	CenterWindow();
	Invalidate();
	return TRUE;
}

void CSkyChat::InitSkinFont()
{
	CString path;

	m_CalledFont.SetChGapPixel( 2 );

	path.Format( "%s\\03_09_num_0.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '0', 9, 16, path);
	path.Format( "%s\\03_09_num_1.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '1', 9, 16, path);
	path.Format( "%s\\03_09_num_2.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '2', 9, 16, path);
	path.Format( "%s\\03_09_num_3.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '3', 9, 16, path);
	path.Format( "%s\\03_09_num_4.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '4', 9, 16, path);
	path.Format( "%s\\03_09_num_5.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '5', 9, 16, path);
	path.Format( "%s\\03_09_num_6.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '6', 9, 16, path);
	path.Format( "%s\\03_09_num_7.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '7', 9, 16, path);
	path.Format( "%s\\03_09_num_8.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '8', 9, 16, path);
	path.Format( "%s\\03_09_num_9.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '9', 9, 16, path);
	path.Format( "%s\\03_09_num_sla.bmp", CHATTING_DIR );	
	m_CalledFont.SetFontCh( '-', 9, 16, path);	
}

void CSkyChat::InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, CString &FileName )
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

void CSkyChat::OnBnClickedBtnExit()
{
	pChatDlg = NULL;
	//if( g_PhoneDlg )
	//    g_PhoneDlg->SendMessage(WM_CHAT_DESTROY);
    DestroyWindow();
    delete chatdlg;
	chatdlg = NULL;
	//CDialog::OnOK();
}

void CSkyChat::OnBnClickedBtnSend()
{
	int nInx;
	char sVal[128], szTmp[512];
    //u_input_str
	if( GetDlgItemText(IDC_EDT_TALK, u_input_str, 512) > 0 )
	{
		memset(sVal, 0, 128);
		GetPrivateProfileString(PHONE_NAME,"E164Prefix", "1000000", sVal, 24, "./phone.ini");
		//LGetPrivateProfileString("E164Prefix", "1000000", sVal, 24, SIPCALLER_CONF);
		sprintf( szTmp, "[%s] : %s", sVal, u_input_str );

		//m_lstWnd.textColor( RGB(0, 0, 255) );	
		nInx = m_lstWnd.AddString(szTmp, RGB(0, 0, 255));		
		m_lstWnd.SetCurSel(nInx);
		m_talk.SetWindowText("");
		if( g_PhoneDlg )
		    g_PhoneDlg->SendMessage(WM_CHAT_MSGSD);

		m_strMsg = "";
		UpdateData(FALSE);
	}
}

BOOL CSkyChat::PreTranslateMessage(MSG* pMsg)
{
	if((GetFocus()==GetDlgItem(IDC_EDT_TALK)))
	{
		if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		{
			OnBnClickedBtnSend();
			return TRUE;
		}
    }
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CSkyChat::OnMsgChatRcv(WPARAM wParam,LPARAM lParam)
{	
    //m_view_wnd.SetWindowText(g_PhoneDlg->chat_msg);	
	int	nInx;
	char szTmp[512];
	

	CUbiDialerDlg *pDlg = APP->m_pUbiDialerDlg;
#ifdef SIP_UA
	sprintf( szTmp, "[%s] : %s",  pDlg->m_pPhone->CallInfo.target_number, g_PhoneDlg->chat_msg );
#endif
	//m_lstWnd.textColor( RGB(192, 192, 192) );
	nInx = m_lstWnd.AddString( szTmp, RGB(192, 192, 192) );
	m_lstWnd.SetCurSel(nInx);

	return NULL;
}

void CSkyChat::OnBnClickedBtnSymbol1()
{
#ifdef USE_SYMBOL_DLG
	CUbiDialerDlg *pDlg = APP->m_pUbiDialerDlg;
#ifdef SIP_UA
	lstrcpy( pDlg->m_pPhone->CallInfo.target_number, ".123456" );
#endif
	lstrcpy( g_PhoneDlg->chat_msg, ".123456" );
	OnMsgChatRcv(0,0);
	CSymbolDlg	dlg;
	dlg.DoModal();
#endif //#ifdef USE_SYMBOL_DLG
}

void CSkyChat::InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, CSkinFont *SkinFont )
{
    UINT nStyle = Label.GetButtonStyle();
    Label.SetButtonStyle(nStyle | BS_OWNERDRAW);    // OWNERDRAW 속성 추가

	Label.SetSkinFont( SkinFont );
	Label.SizeToContent( x, y, w, h, TRUE );
}

void CSkyChat::InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, t_Font *SkinFont )
{
    UINT nStyle = Label.GetButtonStyle();
    Label.SetButtonStyle(nStyle | BS_OWNERDRAW);    // OWNERDRAW 속성 추가

	Label.SetSkinFont( SkinFont );
	Label.SizeToContent( x, y, w, h, TRUE );
}
int CSkyChat::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	pChatDlg = this;
	// TODO: Add extra initialization here
	//SetDlgItemText(IDC_EDT_TALK, "startCHAT");
	//OnBnClickedBtnSend();
	//init_skin();	
	return 0;
}

void CSkyChat::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnCancel();
}

BOOL CSkyChat::DestroyWindow()
{
//	if( g_PhoneDlg )
//	    g_PhoneDlg->SendMessage(WM_CHAT_DESTROY);
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CDialog::DestroyWindow();
}
