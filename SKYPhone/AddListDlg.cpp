// AddListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ubidialer.h"
#include "AddListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddListDlg dialog


CAddListDlg::CAddListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddListDlg)
	m_szGroup = _T("");
	m_szHandPhoneNo = _T("");
	m_szHomePhoneNo = _T("");
	m_szPerson = _T("");
	m_szUbiPhoneNo = _T("");
	//}}AFX_DATA_INIT
}


void CAddListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddListDlg)
	DDX_Control(pDX, IDC_LBL_GROUP, m_lblGroup);
	DDX_Control(pDX, IDC_EDT_UBI_PHONE, m_edtUbiPhone);
	DDX_Control(pDX, IDC_EDT_PERSON, m_edtPerson);
	DDX_Control(pDX, IDC_EDT_HOME_PHONE, m_edtHomePhone);
	DDX_Control(pDX, IDC_EDIT_HAND_PHONE, m_edtHandPhone);
	DDX_Control(pDX, IDC_BTN_OK, m_btnOK);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_CANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_LBL_GROUP, m_szGroup);
	DDX_Text(pDX, IDC_EDIT_HAND_PHONE, m_szHandPhoneNo);
	DDV_MaxChars(pDX, m_szHandPhoneNo, 20);
	DDX_Text(pDX, IDC_EDT_HOME_PHONE, m_szHomePhoneNo);
	DDV_MaxChars(pDX, m_szHomePhoneNo, 20);
	DDX_Text(pDX, IDC_EDT_PERSON, m_szPerson);
	DDV_MaxChars(pDX, m_szPerson, 50);
	DDX_Text(pDX, IDC_EDT_UBI_PHONE, m_szUbiPhoneNo);
	DDV_MaxChars(pDX, m_szUbiPhoneNo, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddListDlg, CDialog)
	//{{AFX_MSG_MAP(CAddListDlg)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddListDlg message handlers
BOOL CAddListDlg::OnEraseBkgnd(CDC* pDC) 
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
UINT CAddListDlg::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	UINT    nReturn = CDialog::OnNcHitTest(point);

    if(nReturn == HTCLIENT)	nReturn = HTCAPTION;  

    return nReturn;  
}

HBRUSH CAddListDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if ( nCtlColor != CTLCOLOR_EDIT    )	{
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CAddListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_szGroup.Format( "%s", m_szSelGroup );
	m_szPerson.Format( "%s", m_szSelPerson );
	m_szHomePhoneNo.Format( "%s", m_szSelPhoneNo[0] );
	m_szHandPhoneNo.Format( "%s", m_szSelPhoneNo[1] );
	m_szUbiPhoneNo.Format( "%s", m_szSelPhoneNo[2] );
	UpdateData( FALSE );
	
	InitSkin();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddListDlg::InitSkinHomeDir()
{
	FILE	*fp;

	strcpy( m_SkinDir, ".\\bmp" );

	if ( ( fp = fopen( "Skin.Dir", "r" ) ) == NULL )	return;
	fscanf( fp, "%s", m_SkinDir );
	fclose( fp );
}

BOOL CAddListDlg::InitSkinBkGnd( char *BkGndFile )
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

void CAddListDlg::InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, 
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

void CAddListDlg::InitSkin()
{
	InitSkinHomeDir();
	if ( !InitSkinBkGnd( "03_sub\\03_04_add list\\03_04_00_bg.bmp" ) )	return;
	
//Close
	InitSkinBtn( m_btnClose, 268, 10, 29, 28,
				"03_sub\\03_00_button\\03_00_00_close_n.bmp",
				"03_sub\\03_00_button\\03_00_00_close_p.bmp",
				"03_sub\\03_00_button\\03_00_00_close_o.bmp" );
//OK
	InitSkinBtn( m_btnOK, 173, 308, 60, 30,
				"03_sub\\03_00_button\\03_00_08_ok_n.bmp",
				"03_sub\\03_00_button\\03_00_08_ok_p.bmp",
				"03_sub\\03_00_button\\03_00_08_ok_o.bmp" );
//Cancel
	InitSkinBtn( m_btnCancel, 238, 308, 60, 30,
				"03_sub\\03_00_button\\03_00_09_cancel_n.bmp",
				"03_sub\\03_00_button\\03_00_09_cancel_p.bmp",
				"03_sub\\03_00_button\\03_00_09_cancel_o.bmp" );
//PersonNameEdit
	m_edtPerson.MoveWindow( 130, 58, 150, 16, TRUE );
//MobilePhoneNo
	m_edtHandPhone.MoveWindow( 130, 104, 150, 16, TRUE );
//HomePhoneNo
	m_edtHomePhone.MoveWindow( 130, 150, 150, 16, TRUE );
//UbiPhoneNo
	m_edtUbiPhone.MoveWindow( 130, 196, 150, 16, TRUE );
//Group
	m_lblGroup.MoveWindow( 30, 270, 245, 16, TRUE );
}

void CAddListDlg::OnBtnClose() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CAddListDlg::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CAddListDlg::OnBtnOk() 
{
	// TODO: Add your control notification handler code here
	char	PhoneNo[3][PHONE_NUMBER_SIZE+1];

	UpdateData( TRUE );

	m_szPerson.TrimLeft();		m_szPerson.TrimRight();
	m_szHomePhoneNo.TrimLeft();	m_szHomePhoneNo.TrimRight();
	m_szHandPhoneNo.TrimLeft();	m_szHandPhoneNo.TrimRight();
	m_szUbiPhoneNo.TrimLeft();	m_szUbiPhoneNo.TrimRight();

	strcpy( PhoneNo[0], (char *)(LPCTSTR)m_szHomePhoneNo );
	strcpy( PhoneNo[1], (char *)(LPCTSTR)m_szHandPhoneNo );
	strcpy( PhoneNo[2], (char *)(LPCTSTR)m_szUbiPhoneNo );
	
	g_PhoneBookDlg->AddPhoneBook( (char *)(LPCTSTR)m_szGroup, 
				(char *)(LPCTSTR)m_szPerson, PhoneNo, 1 );

	OnOK();
}

void CAddListDlg::SetGroup( char *szGroup )
{
	strcpy( m_szSelGroup, szGroup );
}

void CAddListDlg::SetData( char *szGroup, char *szPerson, char szPhoneNo[][PHONE_NUMBER_SIZE+1] )
{
	strcpy( m_szSelGroup, szGroup );
	strcpy( m_szSelPerson, szPerson );
	strcpy( m_szSelPhoneNo[0], szPhoneNo[0] );
	strcpy( m_szSelPhoneNo[1], szPhoneNo[1] );
	strcpy( m_szSelPhoneNo[2], szPhoneNo[2] );
}
