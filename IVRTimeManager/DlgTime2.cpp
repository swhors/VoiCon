// DlgTime2.cpp : implementation file
//

#include "stdafx.h"
#include "IVRTimeManager.h"
#include "DlgTime2.h"

extern int dlg_type;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTime2 dialog


CDlgTime2::CDlgTime2(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTime2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTime2)
	//}}AFX_DATA_INIT
}


void CDlgTime2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTime2)
	DDX_Control(pDX, IDC_YEAR, m_edYear);
	DDX_Control(pDX, IDC_DAY, m_edDay);
	DDX_Control(pDX, IDC_MONTH, m_edMonth);
	DDX_Control(pDX, IDC_MINUTE, m_edMinute);
	DDX_Control(pDX, IDC_HOUR, m_edHour);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTime2, CDialog)
	//{{AFX_MSG_MAP(CDlgTime2)
	ON_EN_CHANGE(IDC_YEAR, OnChangeYear)
	ON_EN_CHANGE(IDC_MONTH, OnChangeMonth)
	ON_EN_CHANGE(IDC_DAY, OnChangeDay)
	ON_EN_CHANGE(IDC_HOUR, OnChangeHour)
	ON_EN_CHANGE(IDC_MINUTE, OnChangeMinute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTime2 message handlers

void CDlgTime2::OnOK() 
{
	BOOL bError=FALSE;
	// TODO: Add extra validation here
	int iYear,iMonth,iDay,iHour,iMinute;
	char tbuf[16];
	this->m_edYear.GetWindowText(tbuf, 16);
	iYear = atoi(tbuf);
	this->m_edMonth.GetWindowText(tbuf, 16);
	iMonth = atoi(tbuf);
	this->m_edDay.GetWindowText(tbuf, 16);
	iDay = atoi(tbuf);
	this->m_edHour.GetWindowText(tbuf, 16);
	iHour = atoi(tbuf);
	this->m_edMinute.GetWindowText(tbuf, 16);
	iMinute = atoi(tbuf);
	if( dlg_type == 0 )
	{
		if( this->m_year > iYear )
		{
			AfxMessageBox("Error : Year is uncorrect.");
			bError = TRUE;
		}
		else
		{
			if( this->m_year == iYear && this->m_month > iMonth )
			{
				AfxMessageBox("Error : Month is uncorrect.");
				bError = TRUE;
			}
			else
			{
				
				if( this->m_hour > iHour 
					&& this->m_year == iYear 
					&& this->m_month == iMonth )
				{
					AfxMessageBox("Error : Hour is uncorrect.");
					bError = TRUE;
				}
				else
				{
					if( this->m_day > iDay
						&& this->m_hour == iHour 
						&& this->m_year == iYear 
						&& this->m_month == iMonth )
					{
						AfxMessageBox("Error : Day is uncorrect.");
						bError = TRUE;
					}
					else
					{
						if( this->m_minute > iMinute
							&& this->m_day == iDay
							&& this->m_hour == iHour 
							&& this->m_year == iYear 
							&& this->m_month == iMonth )
						{
							AfxMessageBox("Error : Minute is uncorrect.");
							bError = TRUE;
						}
					}
				}
			}
		}
	}
	if( bError )
		CDialog::OnCancel();
	if( dlg_type == 0 )
	{
		this->m_day = iDay;
		this->m_year = iYear;
		this->m_month = iMonth;
	}
	else
	{
		this->m_day = 0;
		this->m_year = 0;
		this->m_month = 0;
	}
	this->m_minute = iMinute;
	this->m_hour = iHour;
	CDialog::OnOK();
}

BOOL CDlgTime2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SYSTEMTIME	sysTime;
	::GetLocalTime(&sysTime);
	this->m_day = sysTime.wDay;
	this->m_year = sysTime.wYear;
	this->m_minute = sysTime.wMinute;
	this->m_hour = sysTime.wHour;
	this->m_month = sysTime.wMonth;
	// TODO: Add extra initialization here
	char tbuf[32];
	if( dlg_type == 0 )
	{
		wsprintf(tbuf, "%d", this->m_day);
		this->m_edDay.SetWindowText(tbuf);
		wsprintf(tbuf, "%d", this->m_year);
		this->m_edYear.SetWindowText(tbuf);
		wsprintf(tbuf, "%d", this->m_month);
		this->m_edMonth.SetWindowText(tbuf);
	}
	else
	{
		this->m_edDay.EnableWindow(FALSE);
		this->m_edMonth.EnableWindow(FALSE);
		this->m_edYear.EnableWindow(FALSE);
	}
	wsprintf(tbuf, "%d", this->m_minute);
	this->m_edMinute.SetWindowText(tbuf);
	wsprintf(tbuf, "%d", this->m_hour);
	this->m_edHour.SetWindowText(tbuf);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTime2::OnChangeYear() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
}

void CDlgTime2::OnChangeMonth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CDlgTime2::OnChangeDay() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CDlgTime2::OnChangeHour() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CDlgTime2::OnChangeMinute() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
