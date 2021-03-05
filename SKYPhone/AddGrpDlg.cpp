// AddGrpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ubidialer.h"
#include "AddGrpDlg.h"
#include "PhoneBookDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddGrpDlg dialog


CAddGrpDlg::CAddGrpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddGrpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddGrpDlg)
	m_szGroup = _T("");
	//}}AFX_DATA_INIT
}


void CAddGrpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddGrpDlg)
	DDX_Control(pDX, IDC_EDT_GROUP, m_edtGroup);
	DDX_Control(pDX, IDC_BTN_OK, m_btnOK);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_CANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_EDT_GROUP, m_szGroup);
	DDV_MaxChars(pDX, m_szGroup, 50);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddGrpDlg, CDialog)
	//{{AFX_MSG_MAP(CAddGrpDlg)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddGrpDlg message handlers

BOOL CAddGrpDlg::OnEraseBkgnd(CDC* pDC) 
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
UINT CAddGrpDlg::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	UINT    nReturn = CDialog::OnNcHitTest(point);

    if(nReturn == HTCLIENT)	nReturn = HTCAPTION;  

    return nReturn;  
}

HBRUSH CAddGrpDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if ( pWnd != &m_edtGroup )	{
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CAddGrpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	InitSkin();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddGrpDlg::InitSkinHomeDir()
{
	FILE	*fp;

	strcpy( m_SkinDir, ".\\bmp" );

	if ( ( fp = fopen( "Skin.Dir", "r" ) ) == NULL )	return;
	fscanf( fp, "%s", m_SkinDir );
	fclose( fp );
}

BOOL CAddGrpDlg::InitSkinBkGnd( char *BkGndFile )
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

void CAddGrpDlg::InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, 
							char *UpFile, char *DownFile, char *OverFile )
{
	char	UpFileName[200], DownFileName[200], OverFileName[200];

    UINT nStyle = Btn.GetButtonStyle();
    Btn.SetButtonStyle(nStyle | BS_OWNERDRAW);    // OWNERDRAW 속성 추가
	
	sprintf( UpFileName, "%s\\%s", m_SkinDir, UpFile );
	sprintf( DownFileName, "%s\\%s", m_SkinDir, DownFile );
	sprintf( OverFileName, "%s\\%s", m_SkinDir, OverFile );
	Btn.LoadBitmapFiles( UpFileName, DownFileName, OverFileName );
	Btn.SizeToContent( x, y, w, h, TRUE );
}

void CAddGrpDlg::InitSkin()
{
	InitSkinHomeDir();
	if ( !InitSkinBkGnd( "03_sub\\03_05_add group\\03_05_00_bg.bmp" ) )	return;
	
//Close
	InitSkinBtn( m_btnClose, 268, 10, 29, 28,
				"03_sub\\03_00_button\\03_00_00_close_n.bmp",
				"03_sub\\03_00_button\\03_00_00_close_p.bmp",
				"03_sub\\03_00_button\\03_00_00_close_o.bmp" );
//OK
	InitSkinBtn( m_btnOK, 173, 124, 60, 30,
				"03_sub\\03_00_button\\03_00_08_ok_n.bmp",
				"03_sub\\03_00_button\\03_00_08_ok_p.bmp",
				"03_sub\\03_00_button\\03_00_08_ok_o.bmp" );
//Cancel
	InitSkinBtn( m_btnCancel, 237, 124, 60, 30,
				"03_sub\\03_00_button\\03_00_09_cancel_n.bmp",
				"03_sub\\03_00_button\\03_00_09_cancel_p.bmp",
				"03_sub\\03_00_button\\03_00_09_cancel_o.bmp" );
//GroupNameEdit
	m_edtGroup.MoveWindow( 30, 85, 250, 16, TRUE );
}

void CAddGrpDlg::OnBtnClose() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CAddGrpDlg::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CAddGrpDlg::OnBtnOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	m_szGroup.TrimLeft();	m_szGroup.TrimRight();
	if ( m_szGroup.IsEmpty() )	return;

	g_PhoneBookDlg->AddPhoneBook( (char *)(LPCTSTR)m_szGroup, NULL, NULL, 1 );

	OnOK();
}
