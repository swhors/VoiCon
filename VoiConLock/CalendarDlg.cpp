// CalendarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VoiConLock.h"
#include "CalendarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarDlg dialog


CCalendarDlg::CCalendarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalendarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCalendarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCalendarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarDlg)
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_calendar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalendarDlg, CDialog)
	//{{AFX_MSG_MAP(CCalendarDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

int GetOSVersion()
{
	DWORD dwVersion,dwBuild;
	DWORD dwWindowsMajorVersion,dwWindowsMinorVersion;
	dwVersion = GetVersion();
	// Get major and minor version numbers of Windows
	dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	// Get build numbers for Windows NT or Win32s
	if (dwVersion < 0x80000000)                // Windows NT
	    dwBuild = (DWORD)(HIWORD(dwVersion));
	else if (dwWindowsMajorVersion < 4)        // Win32s
		dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
	else         // Windows 95 -- No build numbers provided
		dwBuild =  0;
	return dwBuild;
}
/////////////////////////////////////////////////////////////////////////////
// CCalendarDlg message handlers

void CCalendarDlg::OnOK() 
{
	m_calendar.GetCurSel(refDateTime);
	CTime cTime;
	cTime = cTime.GetCurrentTime();
	if( GetOSVersion() < 2500)
	{
		if( refDateTime != cTime )
		{
			refDateTime -= 86400*12;
		}
	}
	CDialog::OnOK();
}

void CCalendarDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
