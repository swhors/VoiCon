// TimeDlg1.cpp : implementation file
//

#include "stdafx.h"
#include "IVRTimeManager.h"
#include "TimeDlg1.h"

extern int dlg_type;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg1 dialog


CTimeDlg1::CTimeDlg1(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeDlg1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeDlg1)
	//}}AFX_DATA_INIT
}


void CTimeDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeDlg1)
	DDX_Control(pDX, IDC_CALENDAR, m_calendar);
	DDX_Control(pDX, IDC_TIME, m_time);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeDlg1, CDialog)
	//{{AFX_MSG_MAP(CTimeDlg1)
	ON_NOTIFY(DTN_CLOSEUP, IDC_CALENDAR, OnCloseupCalendar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg1 message handlers

void CTimeDlg1::OnOK() 
{
	// TODO: Add extra validation here
	char tbuf[64];
	CTime lTime;
	
	this->m_calendar.GetWindowText(tbuf, 64);
	this->csCalendar = tbuf;
	this->csCalendar.Replace("-", " ");
	
	this->m_time.GetWindowText(tbuf, 64);
	this->m_time.GetTime(lTime);
	this->csTime.Format("%02d %02d %02d", lTime.GetHour(), lTime.GetMinute(), lTime.GetSecond() );
	CDialog::OnOK();
}

void CTimeDlg1::OnCloseupCalendar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	char tbuf[64];
	this->m_calendar.GetWindowText(tbuf, 64);
	this->csCalendar = tbuf;
	this->csCalendar.Replace("-", " ");
	*pResult = 0;
}


BOOL CTimeDlg1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	char tbuf[64];
	this->m_calendar.GetWindowText(tbuf, 64);
	this->csCalendar = tbuf;
	this->csCalendar.Replace("-", " ");
	
	CTime lTime;
	this->m_time.GetWindowText(tbuf, 64);
	this->m_time.GetTime(lTime);
	this->csTime.Format("%02d %02d %02d", lTime.GetHour(), lTime.GetMinute(), lTime.GetSecond() );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
