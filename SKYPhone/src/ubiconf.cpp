// ubiconf.cpp : implementation file
//

#include "stdafx.h"
#include "ubidialer.h"
#include "ubiconf.h"

#include "BlowFish.h"

#ifdef SIP_UA
#include "cProfile.h"
#endif

#define SIPCONF_DIR ".\\bmp\\03_sub\\03_15_setupsip\\"

bool win32_getMyIP(int nFamily, char * myip);

#define __FORWARD_USE_ENABLED

int SetPrivateProfileString(IN char *index,IN char *item,IN char *Val,IN int len,IN char *filename);
int SetPrivateProfileInt(IN char *index,IN char *item,IN int iVal,IN char *filename);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUBIConf dialog


CUBIConf::CUBIConf(CWnd* pParent /*=NULL*/)
	: CDialog(CUBIConf::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUBIConf)
	//}}AFX_DATA_INIT
}


void CUBIConf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUBIConf)
	DDX_Control(pDX, IDC_BTN_EXIT, m_exit);
	DDX_Control(pDX, IDCANCEL, m_btn_cancel);
	DDX_Control(pDX, IDOK, m_btn_ok);
	DDX_Control(pDX, IDC_RAS_GK_IP, m_GKIP);
	DDX_Control(pDX, IDC_ED_RESPONSETIMEOUT, m_edResponseTimeOut);
	DDX_Control(pDX, IDC_ED_RAS_PORT, m_edRasPort);
	DDX_Control(pDX, IDC_ED_RAS_E164, m_edRasE164);
	DDX_Control(pDX, IDC_ED_RAS_H323ID, m_edRasH323ID);
	DDX_Control(pDX, IDC_ED_Q931_PORT, m_edQ931Port);
	DDX_Control(pDX, IDC_ED_MAX_AL, m_edMaxAl);
	DDX_Control(pDX, IDC_ED_GK_PORT, m_ed_gkPort);
	DDX_Control(pDX, IDC_CHK_TUNNELING, m_chkTunneling);
	DDX_Control(pDX, IDC_CHK_SilenceSupression, m_chkSilenceSup);
	DDX_Control(pDX, IDC_CHK_G7231, m_chkG7231);
	DDX_Control(pDX, IDC_CHK_G711Ulaw, m_chkG711Ulaw);
	DDX_Control(pDX, IDC_CHK_DTMF, m_chkDTMF);
	DDX_Control(pDX, IDC_CHK_FASTSTART, m_chkFastStart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUBIConf, CDialog)
	//{{AFX_MSG_MAP(CUBIConf)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHK_G7231, OnChkG7231)
	ON_BN_CLICKED(IDC_CHK_FASTSTART, OnChkFaststart)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_CHK_TUNNELING, OnChkTunneling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUBIConf message handlers

void CUBIConf::OnOK() 
{
	// TODO: Add extra validation here
	WriteSetting();
	CDialog::OnOK();
}

void CUBIConf::InitSkin()
{
	CString path;

	InitSkinHomeDir();
	if( !InitSkinBkGnd( "03_sub\\03_15_setupsip\\conf_h323.bmp" ) )	return;
	
	path.Format( "%s\\btn_ok", SIPCONF_DIR );
	InitSkinBtn( m_btn_ok, 175, 498, 57, 30, path );
	path.Format( "%s\\btn_cancel", SIPCONF_DIR );
	InitSkinBtn( m_btn_cancel, 240, 498, 57, 30, path);
    
	path.Format( "%s\\03_00_00_close", SUB_BUTTON_DIR );
	InitSkinBtn( m_exit, 269, 10, 29, 28, path );
	this->m_edRasPort.MoveWindow(133,71, 95, 20);
	this->m_edRasH323ID.MoveWindow(133,93, 115, 20);
	this->m_edRasE164.MoveWindow(133,115,115,20);
	this->m_ed_gkPort.MoveWindow(133, 137, 95, 20);
	this->m_GKIP.MoveWindow(133,159, 115, 20);
	this->m_edQ931Port.MoveWindow(150, 221, 77, 20);
	this->m_edResponseTimeOut.MoveWindow(150,244, 77, 20);
	this->m_chkFastStart.MoveWindow(24, 276, 11, 11);
	this->m_chkTunneling.MoveWindow(24, 297, 11, 11);
	this->m_chkG7231.MoveWindow(29, 363, 11, 11);
	this->m_chkSilenceSup.MoveWindow(64, 386, 11, 11);
	this->m_chkG711Ulaw.MoveWindow(29,426, 11, 11);
	this->m_chkDTMF.MoveWindow(29, 445, 11, 11);
	this->m_edMaxAl.MoveWindow(150,401, 78, 20);
}

void CUBIConf::InitSkinHomeDir()
{
	FILE	*fp;
	strcpy( m_SkinDir, ".\\bmp" );
	if ( ( fp = fopen( "Skin.Dir", "r" ) ) == NULL )	return;
	fscanf( fp, "%s", m_SkinDir );
	fclose( fp );
}

BOOL CUBIConf::InitSkinBkGnd(char *BkGndFile)
{
	CString	path;
	char	FName[200];
    memset(FName, 0, 200);
	sprintf( FName, "%s\\%s", m_SkinDir, BkGndFile );
    if ( !LoadBitmapFile(m_bmpBkGnd, FName ) )	return FALSE;
	BITMAP bmp;
	m_bmpBkGnd.GetBitmap( &bmp );

	SetWindowPos( NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOZORDER );
	BitmapRgn( m_hWnd, (HBITMAP)m_bmpBkGnd, RGB( 255,0,255 ) );
	
	CenterWindow();
	Invalidate();

	return TRUE;
}

UINT CUBIConf::OnNcHitTest(CPoint point)
{
	UINT    nReturn = CDialog::OnNcHitTest(point);

    if(nReturn == HTCLIENT)	nReturn = HTCAPTION;  

    return nReturn;
}

void CUBIConf::InitSkinBtn(CSkinButton &Btn, int x, int y, int w, int h, CString &FileName)
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

BOOL CUBIConf::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitSkin();
	loadSetting();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUBIConf::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}

HBRUSH CUBIConf::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if( pWnd!=&m_ed_gkPort && pWnd!=&m_edMaxAl && pWnd!=&m_edQ931Port
		&& pWnd!=&m_edRasE164 && pWnd!=&m_edRasH323ID && pWnd!=&m_edRasPort
		&& pWnd!=&m_edResponseTimeOut && pWnd!=&m_GKIP && pWnd!=&m_chkDTMF
		&& pWnd!=&m_chkFastStart && pWnd!=&m_chkG711Ulaw && pWnd!=&m_chkG7231
		&& pWnd!=&m_chkSilenceSup && pWnd!=&m_chkTunneling )
	{
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	return hbr;
}

BOOL CUBIConf::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);

	CBitmap* pbmpOld = MemDC.SelectObject( &m_bmpBkGnd );
	pDC->BitBlt(0, 0, rc.right, rc.bottom, &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject( pbmpOld );
	return CDialog::OnEraseBkgnd(pDC);
}

void CUBIConf::WriteSetting()
{
	// RAS Information
	int iVal;
	char tbuf[64];
	this->m_edRasPort.GetWindowText(tbuf,64);
	::WritePrivateProfileString("BISCON_RAS", "RAS_RasPort", tbuf, H323PHONE_INI);

	for(int i = 1; i < 5; i++ )
	{
		wsprintf(tbuf, "CODEC_TYPE%d", i);
		::WritePrivateProfileString("BISCON_RTP", tbuf, "-1", H323PHONE_INI);
	}
	
	// set GK IP
	this->m_GKIP.GetWindowText(tbuf,64);
	::WritePrivateProfileString("BISCON_RAS", "RAS_GKipAddress_ip", tbuf, H323PHONE_INI);
	// set GK Port
	this->m_ed_gkPort.GetWindowText(tbuf,64);
	::WritePrivateProfileString("BISCON_RAS", "RAS_GKipAddress_port", tbuf, H323PHONE_INI);

	// Set H323-ID
	this->m_edRasH323ID.GetWindowText(tbuf,64);
	::WritePrivateProfileString("BISCON_RAS", "H323-ID", tbuf, H323PHONE_INI);

	// set E164
	this->m_edRasE164.GetWindowText(tbuf,64);
	::WritePrivateProfileString("BISCON_RAS", "E164_ID_1", tbuf, H323PHONE_INI);
	
	// set responseTimeOut Value.
	this->m_edResponseTimeOut.GetWindowText(tbuf,64);
	::WritePrivateProfileString("BISCON_Q931", "Q931_ResponseTimeOut", tbuf, H323PHONE_INI);
	
	// set callSignallingPort
	this->m_edQ931Port.GetWindowText(tbuf,64);
	::WritePrivateProfileString("BISCON_Q931", "Q931_CallSignalingPort", tbuf, H323PHONE_INI);
	
	// set h245Tunneling
	iVal = m_chkTunneling.GetCheck();
	wsprintf(tbuf, "%d", iVal);
	::WritePrivateProfileString("BISCON_Q931", "Q931_H245Tunneling", tbuf, H323PHONE_INI);
	iVal = m_chkFastStart.GetCheck();
	wsprintf(tbuf, "%d", iVal);
	::WritePrivateProfileString("BISCON_Q931", "FastStart", tbuf, H323PHONE_INI);
	int iCount=1;
	if( this->m_chkG7231.GetCheck() )
	{
		wsprintf(tbuf, "CODEC_TYPE%d", iCount++);
		::WritePrivateProfileString("BISCON_RTP", tbuf, "0", H323PHONE_INI);
		iVal = this->m_chkSilenceSup.GetCheck();
		wsprintf(tbuf, "%d", iVal);
		::WritePrivateProfileString("BISCON_RTP", "G7231_SilenceSupression", tbuf, H323PHONE_INI);
		iVal = this->m_edMaxAl.GetWindowText(tbuf,64);
		::WritePrivateProfileString("BISCON_RTP", "G7231_maxAl-sduAudioFrames", tbuf, H323PHONE_INI);

	}
	if( this->m_chkG711Ulaw.GetCheck() )
	{
		wsprintf(tbuf, "CODEC_TYPE%d", iCount++);
		::WritePrivateProfileString("BISCON_RTP", tbuf, "1", H323PHONE_INI);
	}
	// dtmf
	iVal = this->m_chkDTMF.GetCheck();
	wsprintf(tbuf, "%d", iVal);
	::WritePrivateProfileString("BISCON_RTP", "DTMF_TYPE", tbuf, H323PHONE_INI);
}

void CUBIConf::loadSetting()
{
	// RAS Information
	int iVal;
	char tbuf[64];
	//iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_ResponseTimeOut", 4, H323PHONE_INI);
	// set RAS Port
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_RasPort", 1719, H323PHONE_INI);
	wsprintf(tbuf, "%d", iVal );
	this->m_edRasPort.SetWindowText(tbuf);
		
	// set GK IP
	::GetPrivateProfileString("BISCON_RAS", "RAS_GKipAddress_ip", "127.0.0.1",tbuf, 64, H323PHONE_INI);
	this->m_GKIP.SetWindowText(tbuf);
	// set GK Port
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_GKipAddress_port", 1719, H323PHONE_INI);
	wsprintf(tbuf,"%d", iVal);
	this->m_ed_gkPort.SetWindowText(tbuf);
	// Set H323-ID
	::GetPrivateProfileString("BISCON_RAS", "H323-ID", "12345", tbuf, 64, H323PHONE_INI);
	this->m_edRasH323ID.SetWindowText(tbuf);
	// set E164
	::GetPrivateProfileString("BISCON_RAS", "E164_ID_1", "12345", tbuf, 64, H323PHONE_INI);
	this->m_edRasE164.SetWindowText(tbuf);

	// set responseTimeOut Value.
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_ResponseTimeOut", 4, H323PHONE_INI);
	wsprintf(tbuf,"%d", iVal);
	this->m_edResponseTimeOut.SetWindowText(tbuf);
	// set callSignallingPort
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_CallSignalingPort", 1720, H323PHONE_INI);
	wsprintf(tbuf,"%d", iVal);
	this->m_edQ931Port.SetWindowText(tbuf);
	// set h245Tunneling
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0, H323PHONE_INI);
	this->m_chkTunneling.SetCheck(iVal);
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "FastStart", 0, H323PHONE_INI);
	this->m_chkFastStart.SetCheck(iVal);
	
	// Set capability
	iVal = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE1", -1, H323PHONE_INI);
	if( iVal == 0 )
	{
		EnableG7231Item();
		// G7231
		this->m_chkG7231.SetCheck(1);
		iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_SilenceSupression", 0, H323PHONE_INI);
		this->m_chkSilenceSup.SetCheck(iVal);
		iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_maxAl-sduAudioFrames", 0, H323PHONE_INI);
		wsprintf(tbuf, "%d", iVal);
		this->m_edMaxAl.SetWindowText(tbuf);

		// check g711
		iVal = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE2", -1, H323PHONE_INI);
		if( iVal >= 1)
			this->m_chkG711Ulaw.SetCheck(1);
	}
	else if( iVal >= 1)
	{
		// g729
		this->m_chkG711Ulaw.SetCheck(1);
		EnableG7231Item(FALSE);
	}
	// dtmf
	iVal = ::GetPrivateProfileInt("BISCON_RTP", "DTMF_TYPE", -1, H323PHONE_INI);
	this->m_chkDTMF.SetCheck(iVal);
}

void CUBIConf::OnChkG7231() 
{
	EnableG7231Item(this->m_chkG7231.GetCheck());
}

void CUBIConf::EnableG7231Item(BOOL bEnable)
{
	this->m_chkSilenceSup.EnableWindow(bEnable);
	this->m_edMaxAl.EnableWindow(bEnable);
	if( !bEnable )
	{
		this->m_chkSilenceSup.SetCheck(0);
		this->m_edMaxAl.SetWindowText("");
	}
}

void CUBIConf::OnChkFaststart() 
{
	if( this->m_chkFastStart.GetCheck() )
		this->m_chkTunneling.SetCheck(0);
	
}

void CUBIConf::OnChkTunneling() 
{
	if( this->m_chkTunneling.GetCheck() )
		this->m_chkFastStart.SetCheck(0);
	
}
