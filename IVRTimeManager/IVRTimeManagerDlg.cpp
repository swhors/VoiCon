// IVRTimeManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IVRTimeManager.h"
#include "IVRTimeManagerDlg.h"
#include "Calendardlg.h"

#include "DlgTime2.h"
#include "TimeDlg1.h"

int dlg_type;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CIVRTimeManagerDlg dialog

CIVRTimeManagerDlg::CIVRTimeManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIVRTimeManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIVRTimeManagerDlg)
	m_start = _T("");
	m_end = _T("");
	m_total = _T("");
	m_PreInfo = _T("");
	m_PreInfo2 = _T("");
	m_start2 = _T("");
	m_end2 = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIVRTimeManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIVRTimeManagerDlg)
	DDX_Control(pDX, IDC_EDIT_PRE2, m_ed_pre2);
	DDX_Control(pDX, IDC_EDIT_PRE, m_ed_pre);
	DDX_Text(pDX, IDC_EDIT1, m_start);
	DDX_Text(pDX, IDC_EDIT2, m_end);
	DDX_Text(pDX, IDC_EDIT_PRE, m_PreInfo);
	DDX_Text(pDX, IDC_EDIT_PRE2, m_PreInfo2);
	DDX_Text(pDX, IDC_EDIT3, m_start2);
	DDX_Text(pDX, IDC_EDIT4, m_end2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIVRTimeManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CIVRTimeManagerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_END, OnBtnEnd)
	ON_BN_CLICKED(IDABOUT, OnAbout)
	ON_BN_CLICKED(IDC_BTN_START2, OnBtnStart2)
	ON_BN_CLICKED(IDC_BTN_END2, OnBtnEnd2)
	ON_COMMAND(IDD_UPDATE_DAILY, OnUpdateDaily)
	ON_COMMAND(IDM_OPEN_WORK, OnOpenWork)
	ON_COMMAND(IDM_SPE_OPEN, OnSpeOpen)
	ON_COMMAND(IDD_NEW_DAILY, OnNewDaily)
	ON_COMMAND(IDC_BTN_NEW, OnBtnNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIVRTimeManagerDlg message handlers

BOOL CIVRTimeManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	dlg_type = 0;
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
	SetIcon(m_hIcon, TRUE);		// Set small icon
	
	pReLoad();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIVRTimeManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CIVRTimeManagerDlg::OnPaint() 
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
HCURSOR CIVRTimeManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CIVRTimeManagerDlg::OnBtnUpdate() 
{
	CString csBuf;
	CFile file;
	CFileException ex;
	CFileDialog dlg(FALSE, "ini", "*.ini", NULL, "ini");
	if( dlg.DoModal() == IDOK )
	{
		csBuf = dlg.GetFileName();
		if( m_start.GetLength() <= 0 || m_end.GetLength() <= 0)
		{
			AfxMessageBox("Fill date information.");
			return;
		}
		if( m_start >= m_end )
		{
			AfxMessageBox("Ending time is not correct.");
			return;
		}
		if( !file.Open(csBuf,CFile::modeWrite|CFile::shareExclusive|CFile::modeCreate, &ex) )
		{
			AfxMessageBox("Can't open file.");
			return;
		}
		csBuf.Format("BEGIN:%s\r\n",m_start);
		file.Write(csBuf,csBuf.GetLength());
		csBuf.Format("END:%s\r\n",m_end);
		file.Write(csBuf,csBuf.GetLength());
		file.Close();
		this->UpdateData(TRUE);
		m_PreInfo.Format("%s -> %s", this->m_start, this->m_end);
		this->UpdateData(FALSE);
	}
}

void CIVRTimeManagerDlg::OnBtnStart() 
{
	int dlgType = GetPrivateProfileInt("TimeManager", "DlgType", 0, "./BisCon.ini");
	dlg_type = 0;
	switch( dlgType )
	{
	case 0:
		{
			CDlgTime2 dlg;
			if( dlg.DoModal() == IDOK )
			{
				this->UpdateData(TRUE);
				this->m_start.Format("%04d %02d %02d %02d %02d 00",
					dlg.m_year,dlg.m_month,dlg.m_day,dlg.m_hour,dlg.m_minute);
				this->UpdateData(FALSE);
			}
		}
		break;
	case 1:
		{
			CTimeDlg1 dlg;
			if( dlg.DoModal() == IDOK )
			{
				this->UpdateData(TRUE);
				this->m_start.Format("%s %s", dlg.csCalendar, dlg.csTime);
				this->UpdateData(FALSE);
			}
		}
		break;
	default:
		{
			CCalendarDlg dlg;

			if( dlg.DoModal() == IDOK )
			{
				this->UpdateData(TRUE);
				this->m_start = dlg.selectedTime;
				this->UpdateData(FALSE);
			}
		}
		break;
	}
}

void CIVRTimeManagerDlg::OnBtnEnd() 
{
	int dlgType = GetPrivateProfileInt("TimeManager", "DlgType", 0, "./BisCon.ini");
	dlg_type = 0;
	switch( dlgType )
	{
	case 0:
		{
			CDlgTime2 dlg;
			if( dlg.DoModal() == IDOK )
			{
				this->UpdateData(TRUE);
				this->m_end.Format("%04d %02d %02d %02d %02d 00",
					dlg.m_year,dlg.m_month,dlg.m_day,dlg.m_hour,dlg.m_minute);
				this->UpdateData(FALSE);
			}
		}
		break;
	case 1:
		{
			CTimeDlg1 dlg;
			if( dlg.DoModal() == IDOK )
			{
				this->UpdateData(TRUE);
				this->m_end.Format("%s %s", dlg.csCalendar, dlg.csTime);
				this->UpdateData(FALSE);
			}
		}
		break;
	default:
		{
			CCalendarDlg dlg;

			if( dlg.DoModal() == IDOK )
			{
				this->UpdateData(TRUE);
				this->m_end = dlg.selectedTime;
				this->UpdateData(FALSE);
			}
		}
		break;
	}
}

void CIVRTimeManagerDlg::OnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CIVRTimeManagerDlg::pReLoad()
{
}

void CIVRTimeManagerDlg::OnBtnStart2() 
{
	CDlgTime2 dlg;
	dlg_type = 1;
	if( dlg.DoModal() == IDOK )
	{
		this->UpdateData(TRUE);
		//this->m_start2.Format("%04d %02d %02d %02d %02d 00",
		//	dlg.m_year,dlg.m_month,dlg.m_day,dlg.m_hour,dlg.m_minute);
		this->m_start2.Format("%02d %02d 00",dlg.m_hour,dlg.m_minute);
		this->UpdateData(FALSE);
	}
}

void CIVRTimeManagerDlg::OnBtnEnd2() 
{
	CDlgTime2 dlg;
	dlg_type = 1;
	if( dlg.DoModal() == IDOK )
	{
		this->UpdateData(TRUE);
		this->m_end2.Format("%02d %02d 00", dlg.m_hour,dlg.m_minute);
		this->UpdateData(FALSE);
	}
}

void CIVRTimeManagerDlg::OnUpdateDaily() 
{
	CString csBuf;
	CFile file;
	CFileException ex;
	CFileDialog dlg(FALSE, "ini", "*.ini", NULL, "ini");
	if( dlg.DoModal() == IDOK )
	{
		csBuf = dlg.GetFileName();
		if( m_start2.GetLength() <= 0 || m_end2.GetLength() <= 0)
		{
			AfxMessageBox("Fill date information.");
			return;
		}
		if( !file.Open(csBuf,CFile::modeWrite|CFile::shareExclusive|CFile::modeCreate, &ex) )
		{
			AfxMessageBox("Can't open file.");
			return;
		}
		csBuf.Format("BEGIN:%s\r\n",m_start2);
		file.Write(csBuf,csBuf.GetLength());
		csBuf.Format("END:%s\r\n",m_end2);
		file.Write(csBuf,csBuf.GetLength());
		file.Close();
		this->UpdateData(TRUE);
		m_PreInfo2.Format("%s -> %s", this->m_start2, this->m_end2);
		this->UpdateData(FALSE);
	}
}

void CIVRTimeManagerDlg::OnOpenWork() 
{
	CString csBuf;
	CStdioFile			file;
	CFileException	ex;
	CFileDialog dlg(TRUE, "ini", "*.ini", NULL, "ini");
	if( dlg.DoModal() == IDOK )
	{
		CString csFileName = dlg.GetFileName();
		this->UpdateData(TRUE);
		// "dailyofftime.ini"
		if( !file.Open(csFileName, CFile::modeRead, &ex) )
		{
			return;
		}
		while(file.ReadString(csBuf))
		{
			if( !csBuf.Find("BEGIN:", 0) )
			{
				csBuf.Delete(0, 6);
				this->m_start2.Format("%s",csBuf);
			}
			else if( !csBuf.Find("END:",0))
			{
				csBuf.Delete(0, 4);
				this->m_end2.Format("%s",csBuf);
			}
		}
		file.Close();
		m_PreInfo2.Format("%s -> %s", this->m_start2, this->m_end2);
		this->UpdateData(FALSE);
	}
}

void CIVRTimeManagerDlg::OnSpeOpen() 
{
	CString csBuf;
	CStdioFile			file;
	CFileException	ex;
	CFileDialog dlg(TRUE, "ini", "*.ini", NULL, "ini");
	if( dlg.DoModal() == IDOK )
	{
		csBuf = dlg.GetFileName();
		this->UpdateData(TRUE);	
		if( !file.Open(csBuf,CFile::modeRead, &ex) )
		{
			return;
		}
		
		while(file.ReadString(csBuf))
		{
			if( !csBuf.Find("BEGIN:", 0) )
			{
				csBuf.Delete(0, 6);
				this->m_start.Format("%s",csBuf);
			}
			else if( !csBuf.Find("END:",0))
			{
				csBuf.Delete(0, 4);
				this->m_end.Format("%s",csBuf);
			}
		}
		file.Close();
		m_PreInfo.Format("%s -> %s", this->m_start, this->m_end);
		this->UpdateData(FALSE);	
	}
}

void CIVRTimeManagerDlg::OnNewDaily() 
{
	this->m_ed_pre2.SetWindowText("");
	this->UpdateData(TRUE);
	this->m_end2="";
	this->m_start2="";
	this->UpdateData(FALSE);
}	
void CIVRTimeManagerDlg::OnBtnNew() 
{
	this->m_ed_pre.SetWindowText("");
	this->UpdateData(TRUE);
	this->m_end="";
	this->m_start="";
	this->UpdateData(FALSE);
}
