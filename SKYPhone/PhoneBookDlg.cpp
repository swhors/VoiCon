// PhoneBookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ubidialer.h"
#include "UbiDialerDlg.h"
#include "PhoneBookDlg.h"
#include "AddGrpDlg.h"
#include "AddListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CPhoneBookDlg	*g_PhoneBookDlg;	

int		g_GroupCnt;
char	g_GroupDB[GROUP_DB_SIZE][GROUP_NAME_SIZE+1];
int		g_PhoneBookCnt;
t_PhoneBook	g_PhoneBookDB[PHONE_BOOK_DB_SIZE];

/////////////////////////////////////////////////////////////////////////////
// CPhoneBookDlg dialog


CPhoneBookDlg::CPhoneBookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPhoneBookDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhoneBookDlg)
	m_szSearch = _T("");
	//}}AFX_DATA_INIT
}


void CPhoneBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhoneBookDlg)
	DDX_Control(pDX, IDC_BTN_B_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_TRASH, m_btnTrash);
	DDX_Control(pDX, IDC_BTN_ERASE, m_btnErase);
	DDX_Control(pDX, IDC_BTN_ADD_ONE, m_btnAddOne);
	DDX_Control(pDX, IDC_BTN_ADD_GROUP, m_btnAddGroup);
	DDX_Control(pDX, IDC_EDT_SEARCH, m_edtSearch);
	DDX_Control(pDX, IDC_BTN_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Text(pDX, IDC_EDT_SEARCH, m_szSearch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhoneBookDlg, CDialog)
	//{{AFX_MSG_MAP(CPhoneBookDlg)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()	
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBtnSearch)
	ON_BN_CLICKED(IDC_BTN_ADD_GROUP, OnBtnAddGroup)
	ON_BN_CLICKED(IDC_BTN_ADD_ONE, OnBtnAddOne)
	ON_BN_CLICKED(IDC_BTN_ERASE, OnBtnErase)
	ON_BN_CLICKED(IDC_BTN_TRASH, OnBtnTrash)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	ON_BN_CLICKED(IDC_BTN_B_CLOSE, OnBtnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhoneBookDlg message handlers


int	CPhoneBookDlg::SearchGroup( char *Group )
{
	int		i;

	for ( i = 0; i < g_GroupCnt; i++ )	{
		if ( !stricmp( g_GroupDB[i], Group ) )	return i;
	}
	return -1;
}

int	CPhoneBookDlg::AddGroup( char *Group )
{
	if ( g_GroupCnt >= GROUP_DB_SIZE )	return -1;
	strcpy( g_GroupDB[g_GroupCnt++], Group );
	return g_GroupCnt-1;
}

int	CPhoneBookDlg::SearchPerson( char *Group, char *Person )
{
	int		GrpInx;

	GrpInx = SearchGroup( Group );
	return ( GrpInx == -1 ) ? -1 : SearchPerson( GrpInx, Person );
}

int	CompGroupPerson( const void *p, const void *q )
{
	int		rc;
	t_PhoneBook	*k1 = (t_PhoneBook *)p;
	t_PhoneBook	*k2 = (t_PhoneBook *)q;

	rc = strcmp( g_GroupDB[ k1->GroupInx ], g_GroupDB[ k2->GroupInx ] );
	if ( rc )	return rc;

	return strcmp( k1->Person, k2->Person );
}

int	CPhoneBookDlg::SearchPerson( int GrpInx, char *Person )
{
	t_PhoneBook	key, *p;

	memset( &key, 0, sizeof( t_PhoneBook ) );
	key.GroupInx = GrpInx;
	if ( Person )	strcpy( key.Person, Person );
	else	key.Person[0] = 0;

	p = (t_PhoneBook *)bsearch( &key, g_PhoneBookDB, g_PhoneBookCnt, sizeof( t_PhoneBook ), CompGroupPerson );
	return p ? ( p - g_PhoneBookDB ) : -1;
}

int	CPhoneBookDlg::DeletePhoneBook( char *Group, char *Person )
{
	int		inx;
	
	inx = SearchPerson( Group, Person );
	if ( inx == -1 )	return -1;
	memmove( g_PhoneBookDB + inx, g_PhoneBookDB + inx + 1, sizeof( t_PhoneBook ) * ( g_PhoneBookCnt - inx - 1 ) );
	memset( g_PhoneBookDB + --g_PhoneBookCnt, 0, sizeof( t_PhoneBook ) );

	return 0;
}

int	CompPhoneBook( const void *p, const void *q )
{
	int		rc;
	t_PhoneBook	*k1 = (t_PhoneBook *)p;
	t_PhoneBook	*k2 = (t_PhoneBook *)q;

	rc = strcmp( g_GroupDB[ k1->GroupInx ], g_GroupDB[ k2->GroupInx ] );
	if ( rc )	return rc;

	rc = strcmp( k1->Person, k2->Person );
	if ( rc )	return rc;

	rc = strcmp( k1->PhoneNo[0], k2->PhoneNo[0] );
	if ( rc )	return rc;

	rc = strcmp( k1->PhoneNo[1], k2->PhoneNo[1] );
	if ( rc )	return rc;

	return strcmp( k1->PhoneNo[2], k2->PhoneNo[2] );
}

int	CPhoneBookDlg::AddPhoneBook( char *Group, char *Person, char PhoneNo[][PHONE_NUMBER_SIZE+1], int SortNeed )
{
	int		GrpInx, PhoneBookInx, Inx;
	
	if ( Group[0] == 0 )	return -1;
	GrpInx = SearchGroup( Group );
	if ( GrpInx == -1 )	{
		GrpInx = AddGroup( Group );
		if ( GrpInx == -1 )		return -1;
	}
	g_PhoneBookDB[g_PhoneBookCnt].GroupInx = GrpInx;

	Inx = PhoneBookInx = SearchPerson( GrpInx, Person );
	if ( PhoneBookInx == -1 )	{
		if ( g_PhoneBookCnt >= PHONE_BOOK_DB_SIZE )	return -1;
		Inx = g_PhoneBookCnt;
	}
	if ( Person )	strcpy( g_PhoneBookDB[Inx].Person, Person );
	else	memset( g_PhoneBookDB[Inx].Person, 0, PERSON_NAME_SIZE + 1 );
	if ( PhoneNo )	{
		strcpy( g_PhoneBookDB[Inx].PhoneNo[0], PhoneNo[0] );	// 0:HOME
		strcpy( g_PhoneBookDB[Inx].PhoneNo[1], PhoneNo[1] );	// 1:HandPhone
		strcpy( g_PhoneBookDB[Inx].PhoneNo[2], PhoneNo[2] );	// 2:PhoneNo
	}
	else	memset( g_PhoneBookDB[Inx].PhoneNo, 0, 3 * ( PHONE_NUMBER_SIZE + 1 ) );

	SetExpandPerson( Group, g_PhoneBookDB[Inx].Person );
	
	if ( PhoneBookInx == -1 )	{
		g_PhoneBookCnt++;
		if ( SortNeed )	qsort( g_PhoneBookDB, g_PhoneBookCnt, sizeof( t_PhoneBook ), CompPhoneBook );
	}

	return 0;
}

void CPhoneBookDlg::SetExpandPerson( char *Group, char *Person )
{
	strcpy( m_szExpandGroup, Group );
	strcpy( m_szExpandPerson, Person );
}

void CPhoneBookDlg::LoadPhoneBook()
{
	int		p;
	FILE	*fp;
	char	szBuff[200];
	char	Group[GROUP_NAME_SIZE+1];
	char	Person[PERSON_NAME_SIZE+1];
	char	PhoneNo[3][PHONE_NUMBER_SIZE+1];

	g_GroupCnt = 0;
	g_PhoneBookCnt = 0;
	if ( (fp = fopen( "PhoneBook.Dat", "r" ) ) == NULL )	return;
	while( !feof( fp ) )	{
		if ( g_PhoneBookCnt >= PHONE_BOOK_DB_SIZE )	break;
		if ( fgets( szBuff, 200, fp ) == NULL )		break;

		p = DataCpy( Group, szBuff + 0, ';', GROUP_NAME_SIZE + 1 );
		p += DataCpy( Person, szBuff + p, ';', PERSON_NAME_SIZE + 1 );
		p += DataCpy( PhoneNo[0], szBuff + p, ';', PHONE_NUMBER_SIZE + 1 );
		p += DataCpy( PhoneNo[1], szBuff + p, ';', PHONE_NUMBER_SIZE + 1 );
		p += DataCpy( PhoneNo[2], szBuff + p, ';', PHONE_NUMBER_SIZE + 1 );

		if ( AddPhoneBook( Group, Person, PhoneNo, 0 ) == -1 )	continue;
	}
	fclose( fp );

	m_szExpandGroup[0] = 0;
	m_szExpandPerson[0] = 0;
	
	qsort( g_PhoneBookDB, g_PhoneBookCnt, sizeof( t_PhoneBook ), CompPhoneBook );
}

void CPhoneBookDlg::SavePhoneBook()
{
	int		i;
	FILE	*fp;

	if ( (fp = fopen( "PhoneBook.Dat", "w" ) ) == NULL )	return;

	qsort( g_PhoneBookDB, g_PhoneBookCnt, sizeof( t_PhoneBook ), CompPhoneBook );

	for ( i = 0; i < g_PhoneBookCnt; i++ )	{
		fprintf( fp, "%-.*s;%-.*s;%-.*s;%-.*s;%-.*s\n",
			GROUP_NAME_SIZE, g_GroupDB[ g_PhoneBookDB[i].GroupInx ],
			PERSON_NAME_SIZE, g_PhoneBookDB[i].Person,
			PHONE_NUMBER_SIZE, g_PhoneBookDB[i].PhoneNo[0],
			PHONE_NUMBER_SIZE, g_PhoneBookDB[i].PhoneNo[1],
			PHONE_NUMBER_SIZE, g_PhoneBookDB[i].PhoneNo[2] );
	}
	fclose( fp );
}

void CPhoneBookDlg::InitSkinHomeDir()
{
	FILE	*fp;

	strcpy( m_SkinDir, ".\\bmp" );

	if ( ( fp = fopen( "Skin.Dir", "r" ) ) == NULL )	return;
	fscanf( fp, "%s", m_SkinDir );
	fclose( fp );
}

BOOL CPhoneBookDlg::InitSkinBkGnd( char *BkGndFile )
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

void CPhoneBookDlg::InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, CString &FileName )
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

void CPhoneBookDlg::InitSkin()
{
	CString	path;

	InitSkinHomeDir();
	if ( !InitSkinBkGnd( "03_sub\\03_02_phonebook\\03_02_00_bg.bmp" ) )	return;	
	
//Close
	path.Format( "%s\\03_00_00_close", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnClose, 328, 10, 29, 28, path );
				
//Search
	path.Format( "%s\\03_00_01_search", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnSearch, 296, 50, 46, 29, path );				
//Add Group
	path.Format( "%s\\03_00_03_add_group", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnAddGroup, 151, 396, 50, 30, path );				
//Add One
	path.Format( "%s\\03_00_04_add_one", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnAddOne, 203, 396, 50, 30, path );				
//Eraser
	path.Format( "%s\\03_00_05_eraser", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnErase, 255, 396, 50, 30, path );				
//Trash
	path.Format( "%s\\03_00_06_junk", SUB_BUTTON_DIR );
	InitSkinBtn( m_btnTrash, 307, 396, 50, 30, path );				
//SearchEdit
	m_edtSearch.MoveWindow( 90, 57, 200, 18, TRUE );
//Tree
	m_Tree.Init( 17, 91, 336, 296, RGB( 200, 200, 200 ) );
	m_Tree.InitImageList( 11, 15 );
	AddImageInTree( "03_sub\\03_02_phonebook\\03_02_folder_close.bmp" );	// 0
	AddImageInTree( "03_sub\\03_02_phonebook\\03_02_folder_open.bmp" );		// 1
	AddImageInTree( "03_sub\\03_02_phonebook\\03_02_icon_line_on.bmp" );	// 2
	AddImageInTree( "03_sub\\03_02_phonebook\\03_02_icon_line_off.bmp" );	// 3
	AddImageInTree( "03_sub\\03_02_phonebook\\03_02_icon_house.bmp" );		// 4
	AddImageInTree( "03_sub\\03_02_phonebook\\03_02_icon_handphone.bmp" );	// 5
	AddImageInTree( "03_sub\\03_02_phonebook\\03_02_icon_ubiphone.bmp" );	// 6
}

BOOL	CPhoneBookDlg::AddImageInTree( char *ImgName )
{
	char	FName[256];

	sprintf( FName, "%s\\%s", m_SkinDir, ImgName );

	return m_Tree.AddImage( FName );
}

HTREEITEM	CPhoneBookDlg::AddGroupTree( char *szGroup )
{
	HTREEITEM	hGroupItem;

	hGroupItem = m_Tree.InsertItem( szGroup, 0, 1, TVI_ROOT );
	if ( !hGroupItem )	return NULL;
	m_Tree.SetItemData( hGroupItem, 0 );
	return hGroupItem;
}

HTREEITEM	CPhoneBookDlg::AddPersonTree( char *szGroup, char *szPerson )
{
	HTREEITEM	hGroupItem;

	if ( !( hGroupItem = SearchItemInTree(TVI_ROOT, szGroup) ) )	return NULL;
	return AddPersonTree( hGroupItem, szPerson );
}

HTREEITEM	CPhoneBookDlg::AddPersonTree( HTREEITEM GroupItem, char *szPerson )
{
	HTREEITEM	hPersonItem;

	hPersonItem = m_Tree.InsertItem( szPerson, 2, 2, GroupItem );
	if ( !hPersonItem )	return NULL;

	m_Tree.SetItemData( hPersonItem, 1 );
	return hPersonItem;
}

HTREEITEM CPhoneBookDlg::AddPhoneNumberTree( 
	char *szGroup,
	char *szPerson,
	char *szPhoneNumber, int PhoneKind )
{
	HTREEITEM	hGroupItem, hPersonItem;
	
	if( !( hGroupItem = SearchItemInTree(TVI_ROOT, szGroup) ) )
		return NULL;
	if( !( hPersonItem = SearchItemInTree(hGroupItem, szPerson) ) )
		return NULL;
	return AddPhoneNumberTree( hPersonItem, szPhoneNumber, PhoneKind );
}

HTREEITEM CPhoneBookDlg::AddPhoneNumberTree(
	HTREEITEM PersonItem,
	char *szPhoneNumber,
	int PhoneKind )
{
	HTREEITEM	hPhoneNoItem;

	hPhoneNoItem = m_Tree.InsertItem( szPhoneNumber, PhoneKind, PhoneKind, PersonItem );
	if( !hPhoneNoItem )
		return NULL;
	m_Tree.SetItemData( hPhoneNoItem, MAKELONG( 2, PhoneKind - PK_HOUSE_PHONE ));
	return hPhoneNoItem;
}

HTREEITEM CPhoneBookDlg::SearchItemInTree( HTREEITEM Parent, char *szItem )
{
	HTREEITEM hItem = m_Tree.GetChildItem( Parent );

	while (hItem != NULL)
	{
		if ( !strcmp( (LPCTSTR)m_Tree.GetItemText( hItem ), szItem ) )
			return hItem;
		hItem = m_Tree.GetNextSiblingItem( hItem );
	}

	return NULL;
}

void CPhoneBookDlg::DisplayTree()
{
	int		i, j;
	char	Group[GROUP_NAME_SIZE+1];
	char	Person[PERSON_NAME_SIZE+1];
	char	PhoneNo[PHONE_NUMBER_SIZE+1];
	HTREEITEM hGroupItem, hPersonItem, hPhoneItem, hNextItem;

	for ( i = 0; i < g_PhoneBookCnt; i++ )	{
		strcpy( Group, g_GroupDB[ g_PhoneBookDB[i].GroupInx ] );
		if ( !( hGroupItem = SearchItemInTree(TVI_ROOT, Group) ) )	
			hGroupItem = AddGroupTree( Group );
		strcpy( Person, g_PhoneBookDB[i].Person );
		if ( !Person[0] )	continue;
		if ( !( hPersonItem = SearchItemInTree(hGroupItem, Person) ) )	
			hPersonItem = AddPersonTree( hGroupItem, Person );
		
		hPhoneItem = m_Tree.GetChildItem( hPersonItem );
		while( hPhoneItem != NULL )	{
			hNextItem = m_Tree.GetNextSiblingItem( hPhoneItem );
			m_Tree.DeleteItem( hPhoneItem );
			hPhoneItem = hNextItem;
		}
		for ( j = 0; j < 3; j++ )	{
			strcpy( PhoneNo, g_PhoneBookDB[i].PhoneNo[j] );
			if ( PhoneNo[0] )	{
				if ( !( hPhoneItem = SearchItemInTree(hPersonItem, PhoneNo) ) )
					hPhoneItem = AddPhoneNumberTree( hPersonItem, PhoneNo, PK_HOUSE_PHONE + j );
			}
		}
		if ( !strcmp( m_szExpandGroup, Group ) && !strcmp( m_szExpandPerson, Person ) )	{
			m_Tree.Select( hPersonItem, TVGN_CARET );
			m_Tree.Expand( hPersonItem, TVE_EXPAND );

			m_szExpandGroup[0] = 0;
			m_szExpandPerson[0] = 0;
		}
	}
	m_Tree.SortChildren( TVI_ROOT );
}

BOOL CPhoneBookDlg::OnEraseBkgnd(CDC* pDC) 
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
UINT CPhoneBookDlg::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	UINT    nReturn = CDialog::OnNcHitTest(point);

    if(nReturn == HTCLIENT)	nReturn = HTCAPTION;  

    return nReturn;  
}

HBRUSH CPhoneBookDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if ( pWnd != &m_edtSearch )	{
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CPhoneBookDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	g_PhoneBookDlg = this;

	InitSkin();

	LoadPhoneBook();
	DisplayTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPhoneBookDlg::OnBtnClose() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CPhoneBookDlg::OnBtnSearch() 
{
	// TODO: Add your control notification handler code here
	BOOL	start = FALSE;
	HTREEITEM hGroupItem, hPersonItem, hPhoneItem, hItem, hCurrItem;

	UpdateData( TRUE );
	hCurrItem = m_Tree.GetSelectedItem();

	hItem = hGroupItem = m_Tree.GetChildItem( TVI_ROOT );
	while (hGroupItem != NULL)	{
		if ( start && m_Tree.GetItemText( hGroupItem ).Find( m_szSearch ) >= 0 )	goto Search_OK;
		if ( hCurrItem == hItem || !hCurrItem )	start = TRUE;

		hItem = hPersonItem = m_Tree.GetChildItem( hGroupItem );
		while (hPersonItem != NULL)	{
			if ( start && m_Tree.GetItemText( hPersonItem ).Find( m_szSearch ) >= 0  )	goto Search_OK;
			if ( hCurrItem == hItem || !hCurrItem )	start = TRUE;

			hItem = hPhoneItem = m_Tree.GetChildItem( hPersonItem );
			while (hPhoneItem != NULL)	{
				if ( start && m_Tree.GetItemText( hPhoneItem ).Find( m_szSearch ) >= 0 )	goto Search_OK;
				if ( hCurrItem == hItem || !hCurrItem )	start = TRUE;
				hItem = hPhoneItem = m_Tree.GetNextSiblingItem( hPhoneItem );
			}
			hItem = hPersonItem = m_Tree.GetNextSiblingItem( hPersonItem );
		}
		hItem = hGroupItem = m_Tree.GetNextSiblingItem( hGroupItem );
	}

	hItem = hGroupItem = m_Tree.GetChildItem( TVI_ROOT );
	while (hGroupItem != NULL)	{
		if ( start && m_Tree.GetItemText( hGroupItem ).Find( m_szSearch ) >= 0 )	goto Search_OK;
		if ( hCurrItem == hItem || !hCurrItem )	return;

		hItem = hPersonItem = m_Tree.GetChildItem( hGroupItem );
		while (hPersonItem != NULL)	{
			if ( start && m_Tree.GetItemText( hPersonItem ).Find( m_szSearch ) >= 0 )	goto Search_OK;
			if( hCurrItem == hItem || !hCurrItem )
				return;

			hItem = hPhoneItem = m_Tree.GetChildItem( hPersonItem );
			while (hPhoneItem != NULL)
			{
				if ( start && m_Tree.GetItemText( hPhoneItem ).Find( m_szSearch ) >= 0 )
					goto Search_OK;
				if ( hCurrItem == hItem || !hCurrItem )
					return;
				hItem = hPhoneItem = m_Tree.GetNextSiblingItem( hPhoneItem );
			}
			hItem = hPersonItem = m_Tree.GetNextSiblingItem( hPersonItem );
		}
		hItem = hGroupItem = m_Tree.GetNextSiblingItem( hGroupItem );
	}
	return;

Search_OK :
	m_Tree.SelectItem( hItem );
	m_Tree.Expand(hItem, TVE_EXPAND);
}

void CPhoneBookDlg::OnBtnAddGroup() 
{
	// TODO: Add your control notification handler code here
	CAddGrpDlg	dlg;
	
	if( dlg.DoModal() == IDOK )
	{
		DisplayTree();
		SavePhoneBook();
	}
}

void CPhoneBookDlg::OnBtnAddOne() 
{
	// TODO: Add your control notification handler code here
	DWORD	ItemData;
	HTREEITEM	hSelItem, hGrpItem, hPersonItem, hItem=NULL;
	char Group[GROUP_NAME_SIZE+1], Person[PERSON_NAME_SIZE+1], PhoneNo[3][PHONE_NUMBER_SIZE+1];
	CAddListDlg	dlg;

	hSelItem = m_Tree.GetSelectedItem();
	if( !hSelItem )
		return;

	ItemData = m_Tree.GetItemData( hSelItem );

	Person[0] = 0;
	PhoneNo[0][0] = 0;
	PhoneNo[1][0] = 0;
	PhoneNo[2][0] = 0;

	switch( LOWORD( ItemData ))
	{
	case 1 :
		hPersonItem = hSelItem;
		hGrpItem = m_Tree.GetParentItem( hSelItem );
		break;
	case 2 :
		hPersonItem = m_Tree.GetParentItem( hSelItem );
		hGrpItem = m_Tree.GetParentItem( hPersonItem );
		break;
	default :
		hPersonItem = NULL;
		hGrpItem = hSelItem;
		break;
	}

	strcpy( Group, (char *)(LPCTSTR)m_Tree.GetItemText( hGrpItem ) );
	if( hPersonItem )
	{
		WORD	PhoneInx;

		strcpy( Person, (char *)(LPCTSTR)m_Tree.GetItemText( hPersonItem ) );
		hItem = m_Tree.GetChildItem( hPersonItem );
		while (hItem != NULL)
		{
			PhoneInx = HIWORD( m_Tree.GetItemData( hItem ) );
			strcpy( PhoneNo[PhoneInx], (char *)(LPCTSTR)m_Tree.GetItemText( hItem ) );
			hItem = m_Tree.GetNextSiblingItem( hItem );
		}
	}

	if( hItem )
	    dlg.SetGroup( (char *)(LPCTSTR)m_Tree.GetItemText( hItem ) );
	dlg.SetData( Group, Person, PhoneNo );

	if ( dlg.DoModal() == IDOK )
	{
		DisplayTree();
		SavePhoneBook();
	}
}

void CPhoneBookDlg::OnBtnErase() 
{
	// TODO: Add your control notification handler code here
	DWORD	ItemData;
	HTREEITEM	hSelItem, hGrpItem, hPersonItem, hPhoneNoItem, hNextItem;
	char Group[GROUP_NAME_SIZE+1], Person[PERSON_NAME_SIZE+1], szMsg[255];

	hSelItem = m_Tree.GetSelectedItem();
	if( !hSelItem )
		return;

	ItemData = m_Tree.GetItemData( hSelItem );

	switch(LOWORD(ItemData ))
	{
	case 1 :
		hPersonItem = hSelItem;
		hGrpItem = m_Tree.GetParentItem( hSelItem );
		break;
	case 2 :
		hPersonItem = m_Tree.GetParentItem( hSelItem );
		hGrpItem = m_Tree.GetParentItem( hPersonItem );
		break;
	default :
		hPersonItem = NULL;
		hGrpItem = hSelItem;
		if ( m_Tree.GetChildItem( hSelItem ) )
		{
			AfxMessageBox( "개인정보가 등록되어 있습니다.\n개인정보를 먼저 삭제하십시요." );
			return;
		}
		break;
	}

	strcpy( Group, (char *)(LPCTSTR)m_Tree.GetItemText( hGrpItem ) );
	if( hPersonItem )
		strcpy( Person, (char *)(LPCTSTR)m_Tree.GetItemText( hPersonItem ) );
	else
		Person[0] = 0;
	sprintf( szMsg, "%s - %s 를(을) 삭제하시겠습니까?", Group, Person );
	if( AfxMessageBox( szMsg, MB_YESNO ) == IDNO )
		return;
	if ( hPersonItem )
	{
		hPhoneNoItem = m_Tree.GetChildItem( hPersonItem );
		while (hPhoneNoItem != NULL)
		{
			hNextItem = m_Tree.GetNextSiblingItem( hPhoneNoItem );
			m_Tree.DeleteItem( hPhoneNoItem );
			hPhoneNoItem = hNextItem;
		}
	}

	DeletePhoneBook( Group, Person );
	m_Tree.DeleteItem( hSelItem );
	SavePhoneBook();
}

void CPhoneBookDlg::OnBtnTrash() 
{
	OnBtnErase();
}

void CPhoneBookDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
    
	DWORD	ItemData;
	HTREEITEM	hSelItem;

	hSelItem = m_Tree.GetSelectedItem();
	if( !hSelItem )
		return;

	ItemData = m_Tree.GetItemData( hSelItem );

	if(LOWORD( ItemData ) != 2 )
		return;

	int  i, j;
	char szNumber[16];
	CString	Str = m_Tree.GetItemText( hSelItem );

	for( i = j = 0; i < Str.GetLength() && j < 15; i++ )
	{
		if((Str[i] >= '0' && Str[i] <= '9') || Str[i]=='*' || Str[i]=='#')
			szNumber[j++] = Str[i];
	}
	szNumber[j] = 0;
	
	strcpy( g_PhoneDlg->m_szPhone, szNumber );
    
	OnOK();
}
