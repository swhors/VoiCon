// SmsMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ubidialer.h"
#include "UbiDialerDlg.h"
#include "SmsMainDlg.h"
//#include "SymbolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmsMainDlg dialog

CSmsMainDlg *pSmsDlg = NULL;

CSmsMainDlg::CSmsMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmsMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmsMainDlg)
	m_strFrom = _T("");
	m_strTo = _T("");
	m_strMsg = _T("");
	//}}AFX_DATA_INIT
}


void CSmsMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmsMainDlg)
	DDX_Control(pDX, IDC_BTN_SYMBOL, m_btnSymbol);
	DDX_Control(pDX, IDC_EDT_TO, m_edtTo);
	DDX_Control(pDX, IDC_EDT_FROM, m_edtFrom);
	DDX_Control(pDX, IDC_BTN_SEND, m_btnSend);
	DDX_Control(pDX, IDC_MESSAGE_WND, m_edtMesg);
	DDX_Control(pDX, IDC_BTN_S_CLOSE, m_btnClose);
	DDX_Text(pDX, IDC_EDT_FROM, m_strFrom);
	DDX_Text(pDX, IDC_EDT_TO, m_strTo);
	DDX_Text(pDX, IDC_MESSAGE_WND, m_strMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSmsMainDlg, CDialog)
	//{{AFX_MSG_MAP(CSmsMainDlg)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_S_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
	ON_BN_CLICKED(IDC_BTN_SYMBOL, OnBtnSymbol)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsMainDlg message handlers
BOOL CSmsMainDlg::OnEraseBkgnd(CDC* pDC) 
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
UINT CSmsMainDlg::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	UINT    nReturn = CDialog::OnNcHitTest(point);

    if(nReturn == HTCLIENT)	nReturn = HTCAPTION;  

    return nReturn;  
}

HBRUSH CSmsMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if ( pWnd != &m_edtFrom && pWnd != &m_edtTo && pWnd != &m_edtMesg)	{	
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CSmsMainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	pSmsDlg = this;
	InitSkin();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSmsMainDlg::InitSkinHomeDir()
{
	FILE	*fp;

	strcpy( m_SkinDir, ".\\bmp" );

	if ( ( fp = fopen( "Skin.Dir", "r" ) ) == NULL )	return;
	fscanf( fp, "%s", m_SkinDir );
	fclose( fp );
}

BOOL CSmsMainDlg::InitSkinBkGnd( char *BkGndFile )
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

void CSmsMainDlg::InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, CString &FileName )
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

void CSmsMainDlg::InitSkin()
{
	CString path;

	InitSkinHomeDir();
	if ( !InitSkinBkGnd( "03_sub\\03_06_sms\\03_06_00_bg.bmp" ) )	return;
//Send
	path.Format( "%s\\03_00_16_chat_send", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnSend, 307, 311, 50, 30, path );	
//Symbol
	path.Format( "%s\\03_00_10_special", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnSymbol, 14, 311, 50, 30, path );	
	
//Close
	path.Format( "%s\\03_00_00_close", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnClose, 328, 10, 29, 28, path );	
//SMS Message
	m_edtMesg.MoveWindow( 20, 170, 332, 130, TRUE );	
	m_edtMesg.bkColor( RGB(199, 200, 207) );
	m_edtFrom.MoveWindow( 90, 55, 201, 21, TRUE );
	m_edtFrom.bkColor( RGB(239, 240, 243) );
	m_edtTo.MoveWindow( 90, 101, 201, 21, TRUE );
	m_edtTo.bkColor( RGB(239, 240, 243) );
}

void CSmsMainDlg::OnBtnClose() 
{
	pSmsDlg = NULL;
	OnOK();
}

void CSmsMainDlg::OnBtnSend() 
{
	UpdateData(TRUE);
	
	if( (m_strFrom.GetLength() < 1 || m_strFrom.GetLength() > 20 ) )
	{
		AfxMessageBox("회신번호 오류입니다.");
		return;
	}
	if( (m_strTo.GetLength() < 1 || m_strTo.GetLength() > 20))
	{
		AfxMessageBox("수신번호 오류입니다.");
		return;
	}
	if ( m_strMsg.GetLength() < 1 )
	{
		AfxMessageBox("전송할 메시지를 입력해 주세요.");
		return;
	}
	char szFrom[21], szTo[21];
	char szMsg[100] = {0};
	strcpy( szFrom, (char*)(LPCTSTR)m_strFrom );
	strcpy( szTo, (char*)(LPCTSTR)m_strTo );
	wsprintf( szMsg, "%-.80s", m_strMsg );
}

void CSmsMainDlg::OnBtnSymbol() 
{
#ifdef USE_SYMBOL_DLG
	CSymbolDlg	dlg;
	dlg.DoModal();
#endif // #ifdef USE_SYMBOL_DLG
}
