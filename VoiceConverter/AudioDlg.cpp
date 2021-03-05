// AudioDlg.cpp : implementation file
//

#pragma warning (disable : 4786)

#include "stdafx.h"
#include "Audio.h"
#include "AudioDlg.h"
#include <io.h>

#include "ProgressDlg.h"

#include <vector>
#include <algorithm>

#include "utilG729.h"

#include "Wave.h"

CWave *cWave;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_UPDATE_TEXT WM_USER+1

int bDescendSort;

std::vector<std::string> selectedItem;

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
	virtual BOOL OnInitDialog();
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioDlg dialog

CAudioDlg::CAudioDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAudioDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	outputSubType = -1;
	outputType = -1;
	inputType = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CAudioDlg::~CAudioDlg()
{
	if( szSrcdataBuffer )
		free(szSrcdataBuffer);
	if( szDsdataBuffer )
		free( szDsdataBuffer);
}

void CAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioDlg)
	DDX_Control(pDX, IDC_CB_DRIVE, m_cb_drive);
	DDX_Control(pDX, IDC_FILE, m_idc_file);
	DDX_Control(pDX, IDC_LIST_PATH, m_list_path);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAudioDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UPDATE_TEXT, UpdateItemText)
	ON_COMMAND(IDM_ABOUT,OnBtnAbout)
	ON_LBN_DBLCLK(IDC_LIST_PATH, OnDblclkListPath)
	ON_NOTIFY(NM_RCLICK, IDC_FILE, OnRclickFile)
	ON_CBN_SELCHANGE(IDC_CB_DRIVE, OnSelchangeCbDrive)
	ON_COMMAND(ID_MENU_W_711A_R, OnMenuSel)
	ON_COMMAND(IDM_ALL,OnBtnAll)
	ON_COMMAND(IDM_G729A, OnG729a)
	ON_BN_CLICKED(IDC_BTN_G729_ALL, OnBtnG729All)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FILE, OnColumnclickFile)
	ON_COMMAND(ID_MENU_W_711U_R, OnMenuSel)
	ON_COMMAND(ID_MENU_W_7231_R, OnMenuSel)
	ON_COMMAND(IDM_G7231_WAV, OnMenuSel)
	ON_COMMAND(IDM_EXIT, OnOK)
	ON_COMMAND(IDM_DELETE, OnDelete)
	ON_COMMAND(IDM_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioDlg message handlers

BOOL CAudioDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	bDescendSort = 0;
	iSelectedItem = -1;
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	iTemCount = 0;
	m_hStream = NULL;
	iSrcLength = 0;
	szSrcdataBuffer = NULL;
	szDsdataBuffer = NULL;
	m_hDriver = NULL;
	DeviceId = NULL;
	cWave =NULL;
	// file list view
	m_cb_drive.ResetContent();
	m_cb_drive.Dir(DDL_DRIVES ,_T(""));
	memset(lpszOldPath, 0, MAX_PATH);
	TCHAR lpszCurPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, lpszCurPath);
	CString csbuf;
	cCurDrive = lpszCurPath[0]+32;
	csbuf.Format("[-%c-]", cCurDrive);
	m_cb_drive.SetWindowText(csbuf);
	filelistInit();
	FindFile(lpszCurPath);
	
	//get_acm_info();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAudioDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAudioDlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAudioDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAudioDlg::StartWaveToGXX()
{
	CString csbuf;
	if( outputType >= 0 && outputType <=2)
	{
		if( outputSubType == 0)
		{
			if( outputType == 0)
			{
				strcat( szDsFile, ".g7231");
			}
			else if ( outputType == 1)
			{
				strcat( szDsFile, ".g711u");
			}
			else if ( outputType == 2)
			{
				strcat( szDsFile, ".g711a");
			}
		}
		else
			strcat( szDsFile, ".wav");
		if( outputType == 0)
		{
			CAcmG7231 g7231(this->szSrcFile);
			if( g7231.open() )
			{
				g7231.SetDESFileName(szDsFile);
				g7231.convert();
				g7231.close();
			}
		}
		else if( outputType == 1)
		{
			CAcmG711u g711(this->szSrcFile);
			if( g711.open() )
			{
				g711.SetDESFileName(szDsFile);
				g711.convert();
				g711.close();
			}
		}
		else if( outputType == 2)
		{
			CAcmG711a g711(this->szSrcFile);
			if( g711.open() )
			{
				g711.SetDESFileName(szDsFile);
				g711.convert();
				g711.close();
			}
		}
	}
	else if( outputType == 3 )
	{
		strcat( szDsFile, ".g729a");
		FileEncodeG729(szSrcFile , szDsFile);
	}
	//this->FileDir();
	return;
}

void CAudioDlg::get_acm_info()
{
	// Get the ACM version
#if 0
	CString csbuf;
	DWORD dwACMVer = acmGetVersion();
	csbuf.Format("ACM version %u.%.02u build %u", HIWORD(dwACMVer) >> 8, HIWORD(dwACMVer) & 0x00ff, LOWORD(dwACMVer));
	SetDlgItemText(ACM_INFO_VER, csbuf);
	if( LOWORD(dwACMVer) == 0)
		AfxMessageBox("Retail");
	
	// Show some ACM metrics
	DWORD dwCodecs = 0;
	MMRESULT mmr = acmMetrics(NULL, ACM_METRIC_COUNT_CODECS, &dwCodecs);
	if( mmr ){
		AfxMessageBox("error");
	} else {
		csbuf.Format("%lu codecs installed\n",dwCodecs);
		SetDlgItemText(ACM_INFO_METRICS, csbuf);
	}
#endif
}

LRESULT CAudioDlg::UpdateItemText(WPARAM wParam,LPARAM lParam)
{
	SetDlgItemText(wParam, (LPCTSTR) lParam);
	return NULL;
}

void CAudioDlg::OnOK() 
{
	CDialog::OnOK();
}

void CAudioDlg::OnBtnOpenSrc() 
{

}
void CAudioDlg::OnBtnAbout() 
{
	CAboutDlg pdlg;
	pdlg.DoModal();
}

void CAudioDlg::OnDblclkListPath() 
{
	int iSel = m_list_path.GetCurSel();
	CString csbuf;
	TCHAR lpszCurPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, lpszCurPath);
	m_list_path.GetText(iSel, csbuf);
	
	int ilen = strlen( lpszCurPath );
	
	if( !csbuf.Compare(".."))
	{
		for( int i = ilen; i > 0; i--)
		{
			if( lpszCurPath[i] == '\\')
			{
				memset( (char *)&lpszCurPath[i+1], 0, ilen -i-1);
				break;
			}
		}
		
	}
	else if( !csbuf.Compare("."))
	{
	}
	else
	{
		sprintf((char *)&lpszCurPath[ilen], "\\%s", csbuf);
	}
	FindFile(lpszCurPath);
}

void CAudioDlg::FindFile(LPSTR lpszCurPath)
{
	WIN32_FIND_DATA FileData;   // Data structure describes the file found
	HANDLE hSearch;             // Search handle returned by FindFirstFile
	TCHAR szMsg[100];           // String to store the error message
	BOOL bFinished = FALSE;
	
	::SetCurrentDirectory(lpszCurPath);
	SetDlgItemText(IDC_ED_PATH, lpszCurPath);
	hSearch = FindFirstFile (TEXT("*.*"), &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		wsprintf (szMsg, TEXT("No files found."));
		return;
	}
	
	m_idc_file.DeleteAllItems();
	CString csbuf;
	int iCount= 0;
	for( int i = m_list_path.GetCount() -1; i >= 0; i--)
	{
		m_list_path.DeleteString(i);
	}
	while (!bFinished)
	{
		if( FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
			|| FileData.dwFileAttributes == 48)
		{
			m_list_path.AddString(FileData.cFileName);
		}
		else
		{
			CString csName = FileData.cFileName;
			csName.MakeLower();
			if( (csName.Find(".wav", 0 )>=0 )||
				(csName.Find(".g711u", 0 )>=0) ||
				(csName.Find(".g7231", 0 )>=0) ||
				(csName.Find(".g711a", 0 )>=0) )
			{
				
				m_idc_file.InsertItem( iCount, FileData.cFileName, 0);
				csbuf.Format("%d", FileData.nFileSizeLow);
				m_idc_file.SetItemText( iCount, 1,  csbuf);
				iCount++;
			}
		}
		
		if (!FindNextFile (hSearch, &FileData))
		{
			bFinished = TRUE;
			
			if (GetLastError () == ERROR_NO_MORE_FILES)
			{
				wsprintf (szMsg, TEXT("Found all of the files."));
			}
			else
			{
				wsprintf (szMsg, TEXT("Unable to find next file."));
			}
		}
	}
	
	if (!FindClose (hSearch))
	{
		wsprintf (szMsg, TEXT("Unable to close search handle."));
	}
}

void CAudioDlg::filelistInit()
{
	LV_COLUMN Column;
    Column.mask     = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    Column.cx       = 240;
    Column.pszText  = "Name";
    Column.cchTextMax = 200;
    Column.iSubItem = 0;
    m_idc_file.InsertColumn (0, &Column);
    Column.cx       = 70;
    Column.pszText  = "Size";
    m_idc_file.InsertColumn (1, &Column);
    Column.cx       = 70;
    Column.pszText  = "Date";
    m_idc_file.InsertColumn (2, &Column);
}

BOOL IsG7231(char * szFile)
{
	CString csbuf = szFile;
	csbuf.MakeLower();
	if( csbuf.Find("g7231", 0) == -1)
		return FALSE;
	return TRUE;
}

BOOL IsWav(char * szFile)
{
	CString csbuf = szFile;
	csbuf.MakeLower();
	if( csbuf.Find("wav", 0) == -1)
		return FALSE;
	return TRUE;
}

void CAudioDlg::OnSelchangeCbDrive()
{
	
	int iSel = m_cb_drive.GetCurSel();
	CString csbuf, csDrive;
	m_cb_drive.GetLBText(iSel, csbuf);
	char first_drive = 'a';//, tbuf[2];
	csDrive.Empty();
	for( int i = 0; i < 26; i++, first_drive++)
	{
		if( csbuf.Find(first_drive) >= 0 )
		{
			csDrive.Format("%c:\\", first_drive);
			break;
		}
	}
	if(! csDrive.IsEmpty())
	{
		if( cCurDrive != csbuf[2])
		{
			cCurDrive = first_drive;
			if( strlen( lpszOldPath ) != 0)
			{
				csbuf = lpszOldPath;
				csbuf.MakeLower();
				if( csbuf.Find(csDrive, 0) >= 0 )
				{
					::GetCurrentDirectory(MAX_PATH, lpszOldPath);
					::SetCurrentDirectory(csbuf);
				}
				else
				{
					::GetCurrentDirectory(MAX_PATH, lpszOldPath);
					::SetCurrentDirectory(csDrive);
				}
			}
			else
			{
				::GetCurrentDirectory(MAX_PATH, lpszOldPath);
				::SetCurrentDirectory(csDrive);
			}
		}
	}
	this->FileDir();
}

void CAudioDlg::WaveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
#if 0
	WAVE_PLAY_INFO * playInfo = (WAVE_PLAY_INFO *)dwInstance;
	CString csbuf;
	MMRESULT mmr=NULL;
	switch( uMsg)
	{
	case WOM_CLOSE:
		break;
	case WOM_DONE:
		mmr = waveOutUnprepareHeader(playInfo->m_hWaveOut, &playInfo->pwh, sizeof( WAVEHDR));
		if( mmr )
		{
			csbuf.Format("Error : waveOutUnprepareHeader [%d]", mmr);
			AfxMessageBox(csbuf);
		}
		waveOutClose(playInfo->m_hWaveOut);
		break;
	case WOM_OPEN:
		break;
	}
#endif
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	TCHAR szFileName[_MAX_PATH];
	BYTE* m_pVersionInfo;
	int len = 0;
	
	HMODULE hModule = GetModuleHandle(NULL);
		
	memset( szFileName, 0, _MAX_PATH);
	
	if( GetModuleFileName(hModule, szFileName, _MAX_PATH) <= 0)
		return FALSE;
	
	DWORD dwDummyHandle; // will always be set to zero
	len = GetFileVersionInfoSize(szFileName, &dwDummyHandle);
	if (len <= 0)
		return FALSE;
	
	m_pVersionInfo = new BYTE[len]; // allocate version info
	memset(m_pVersionInfo, 0, len);
	if (!::GetFileVersionInfo(szFileName, 0, len, m_pVersionInfo))
		return FALSE;
	unsigned int cbTranslate;
	struct LANGANDCODEPAGE
	{
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	// Read the list of languages and code pages.
	VerQueryValue(m_pVersionInfo, 
		TEXT("\\VarFileInfo\\Translation"),
		(LPVOID*)&lpTranslate,
		&cbTranslate);
	
	for(int i=0; i< (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
	{
		char * pVal;
		UINT iLenVal;
		char SubBlock[256];
		//memset(pVal, 0, 256);
		iLenVal = 256;
		wsprintf( SubBlock, 
            TEXT("\\StringFileInfo\\%04x%04x\\ProductVersion"),
            lpTranslate[i].wLanguage,
            lpTranslate[i].wCodePage);
	  // Retrieve file description for language and code page "i". 
		pVal = new char[256];
		VerQueryValue( m_pVersionInfo, SubBlock, (LPVOID*)&pVal, &iLenVal);
		if( iLenVal > 0 )
		{
			SetDlgItemText(IDC_VERSION, pVal);
			break;
		}
	}
	return TRUE;

}

void CAudioDlg::OnBtnAll() 
{
	char szCurPath[MAX_PATH];
	CFileFind	pFF;
	CString	strPathFile;
	CFile	pReadFile;
	int		nFileLength = 0;
	BOOL	bFind;
	char	tbuf[MAX_PATH];
	memset( szCurPath, 0, MAX_PATH);
	if( GetDlgItemText(IDC_ED_PATH, szCurPath, MAX_PATH) <= 0)
	{
		AfxMessageBox("Invalid Directory Information..");
		return;
	}
	::SetCurrentDirectory(szCurPath);
	strcat(szCurPath, "\\*.wav");
	bFind = pFF.FindFile(szCurPath);
	int iCount =0;
	int iLen = strlen( szCurPath);
	CProgressDlg *pProgress = new CProgressDlg(iCount, szSrcFile);
	memset( (char *)&szCurPath[iLen - 5], 0, 5);
	EnableWindow(FALSE);
	pProgress->Create(IDD_PROGRESS);
	while(bFind)
	{
		iCount ++;
		bFind = pFF.FindNextFile();
		strPathFile = pFF.GetFileName();
		memset(szSrcFile, 0, MAX_PATH);
		sprintf(tbuf, strPathFile);
		sprintf(szSrcFile, "%s%s", szCurPath, tbuf);
		iLen = strlen( tbuf);
		memset( (char *)&tbuf[iLen-4], 0, 4);
		pProgress->UpBar();
		sprintf(szDsFile, tbuf);
		MenuSel(ID_MENU_W_711A_R);
		sprintf(szDsFile, tbuf);
		MenuSel(ID_MENU_W_711U_R);
		sprintf(szDsFile, tbuf);
		MenuSel(ID_MENU_W_7231_R);
		sprintf(szDsFile, tbuf);
		MenuSel(IDM_G729A);
	}
	delete pProgress;
	EnableWindow(TRUE);
	SetForegroundWindow();
}

void CAudioDlg::OnRclickFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int		iItem;
	BOOL	bWave=TRUE;
	char	fileName[MAX_PATH];
	POSITION pos = this->m_idc_file.GetFirstSelectedItemPosition();
	while (pos)
	{
		iItem = this->m_idc_file.GetNextSelectedItem(pos);
		if( this->m_idc_file.GetItemText(iItem,0,fileName, MAX_PATH) )
		{
			if( !IsWav(fileName) )
				bWave = FALSE;
			selectedItem.push_back(fileName);
		}
	}
	if( bWave )
	{
		HMENU hMenu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MENU_WAVE_CON));
		HMENU hSubMenu1 = GetSubMenu(hMenu, 0);
		POINT point;
		::GetCursorPos(&point);
		TrackPopupMenu( hSubMenu1, NULL,point.x,point.y, NULL, m_hWnd, NULL);
		DestroyMenu(hMenu); 
	}
	else
	{
		HMENU hMenu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MENU_CON));
		HMENU hSubMenu1 = GetSubMenu(hMenu, 0);
		POINT point;
		::GetCursorPos(&point);
		TrackPopupMenu( hSubMenu1, NULL,point.x,point.y, NULL, m_hWnd, NULL);
		DestroyMenu(hMenu);
	}
	*pResult = 0;
}

void CAudioDlg::OnRefresh()
{
	this->FileDir();
}

void CAudioDlg::OnMenuSel() 
{
	char *tbuf,*tbuf1;
	std::vector<std::string>::iterator begin,end,cur;
	PMSG pMesg = (PMSG)GetCurrentMessage();
	begin = selectedItem.begin();
	end = selectedItem.end();
	for(cur = begin; cur < end; cur++)
	{
		tbuf = (char*)cur->c_str();
		if( tbuf )
		{
			sprintf(this->szSrcFile, "%s", tbuf);
			sprintf(this->szDsFile,"%s", tbuf);
			tbuf1=strstr(this->szDsFile, ".w");
			if( !tbuf1 )
			{
				tbuf1 = strstr(this->szDsFile,".W");
			}
			if( tbuf1 )
			{
				memset(tbuf1, 0, strlen(tbuf1));
			}
			MenuSel(pMesg->wParam);
		}
	}
	selectedItem.erase(begin,end);
	FileDir();
}


void CAudioDlg::MenuSel(DWORD dwParam)
{
	switch(dwParam)
	{
	case ID_MENU_W_711A_R: // convert wave to g711a, and output file is raw format.
		outputSubType = 0;
		outputType = 2;
		break;
	case ID_MENU_W_711U_R: // convert wave to g711u, and output file is raw format.
		outputSubType = 0;
		outputType = 1;
		break;
	case ID_MENU_W_7231_R: // convert wave to g7231, and output file is raw format.
		outputSubType = 0;
		outputType = 0;
		break;
	case IDM_G729A: // convert wave to g7229, and output file is raw format.
		outputSubType = 0;
		outputType = 3;
		break;
	case IDM_G7231_WAV:
		{
			outputSubType = 1;
			outputType = 0;
		}
		break;
	}
	StartWaveToGXX();
}

void CAudioDlg::OnG729a() 
{
	PMSG pMesg = (PMSG)GetCurrentMessage();
	MenuSel(pMesg->wParam);
}

void CAudioDlg::OnBtnG729All() 
{
#if 0
	char szCurPath[MAX_PATH];
	CFileFind	pFF;
	CString	strPathFile;
	CFile	pReadFile;
	int		nFileLength = 0;
	BOOL	bFind;
	char	tbuf[MAX_PATH];
	memset( szCurPath, 0, MAX_PATH);
	if( GetDlgItemText(IDC_ED_PATH, szCurPath, MAX_PATH) <= 0) {
		AfxMessageBox("Invalid Directory Information..");
		return;
	}
	::SetCurrentDirectory(szCurPath);
	strcat(szCurPath, "\\*.wav");
	bFind = pFF.FindFile(szCurPath);
	int iCount =0;
	int iLen = strlen( szCurPath);
	CProgressDlg *pProgress = new CProgressDlg(iCount, szSrcFile);
	memset( (char *)&szCurPath[iLen - 5], 0, 5);
	EnableWindow(FALSE);
	pProgress->Create(IDD_PROGRESS);
	while(bFind)
	{
		iCount ++;
		bFind = pFF.FindNextFile();
		strPathFile = pFF.GetFileName();
		memset(szSrcFile, 0, MAX_PATH);
		sprintf(tbuf, strPathFile);
		sprintf(szSrcFile, "%s%s", szCurPath, tbuf);
		iLen = strlen( tbuf);
		memset( (char *)&tbuf[iLen-4], 0, 4);
		pProgress->UpBar();
		sprintf(szDsFile, tbuf);
		MenuSel(ID_MENU_W_711A_R);
		sprintf(szDsFile, tbuf);
		MenuSel(ID_MENU_W_711U_R);
		sprintf(szDsFile, tbuf);
		MenuSel(ID_MENU_W_7231_R);
	}
	delete pProgress;
	EnableWindow(TRUE);
	SetForegroundWindow();
#endif
}

int CALLBACK NameCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
	CString    strItem1 = pListCtrl->GetItemText(lParam1, 0);
	CString    strItem2 = pListCtrl->GetItemText(lParam2, 0);
	if( bDescendSort )
		return strcmp(strItem1, strItem2);
	return strcmp(strItem2, strItem1);
}

int CALLBACK SizeCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
	int iSize1 = pListCtrl->GetItemData(lParam1);
	int iSize2 = pListCtrl->GetItemData(lParam2);
	if( iSize1 > iSize2 )
		return 1;
	else if( iSize1 == iSize2 )
		return 0;
	return -1;
}

void CAudioDlg::OnColumnclickFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem < 0 )
	{
		if( pNMListView->iSubItem == 0)
		{
			this->m_idc_file.SortItems((PFNLVCOMPARE)NameCompareProc, (LPARAM)&this->m_idc_file);
			if( bDescendSort )
				bDescendSort = 0;
			else
				bDescendSort = 1;
		}
		else if( pNMListView->iSubItem == 1)
		{
			this->m_idc_file.SortItems(SizeCompareProc, (LPARAM)&this->m_idc_file);
		}
		else if( pNMListView->iSubItem == 2)
		{
		}
	}
	*pResult = 0;
}

void CAudioDlg::OnDelete() 
{
	char tbuf[256];
	if( this->iSelectedItem >= 0 )
	{
		if( AfxMessageBox("This file will be deleted. Do you OK?", MB_OKCANCEL) == IDOK )
		{
			int iItem;
			int iCount = this->m_idc_file.GetSelectedCount();
			this->m_idc_file.GetSelectionMark();
			if( iCount > 0 )
			{
				POSITION pos = this->m_idc_file.GetFirstSelectedItemPosition();
				while (pos)
				{
					iItem = this->m_idc_file.GetNextSelectedItem(pos);
					if( this->m_idc_file.GetItemText(iItem,0,tbuf, 256) )
					{
						TCHAR lpszCurPath[MAX_PATH];
						::GetCurrentDirectory(MAX_PATH, lpszCurPath);
						strcat(lpszCurPath, "\\");
						strcat(lpszCurPath, tbuf);
						::DeleteFile(lpszCurPath);
					}
				}
			}
			FileDir();
		}
	}
}

void CAudioDlg::FileDir()
{
	TCHAR lpszCurPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, lpszCurPath);
	FindFile(lpszCurPath);
}
