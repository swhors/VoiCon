// TimeChkPage1.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "TimeChkPage1.h"

#include "Item.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeChkPage1 property page

IMPLEMENT_DYNCREATE(CTimeChkPage1, CPropertyPage)

CTimeChkPage1::CTimeChkPage1() : CPropertyPage(CTimeChkPage1::IDD)
{
	//{{AFX_DATA_INIT(CTimeChkPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTimeChkPage1::~CTimeChkPage1()
{
}

void CTimeChkPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeChkPage1)
	DDX_Control(pDX, IDC_ED_SFNAME, m_edSFname);
	DDX_Control(pDX, IDC_ED_SPE_GOTO, m_edsgoto);
	DDX_Control(pDX, IDC_CHKSPECIALDAY, m_chkspeday);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeChkPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CTimeChkPage1)
	ON_BN_CLICKED(IDC_CHKSPECIALDAY, OnChkspecialday)
	ON_BN_CLICKED(IDC_BTN_SP_OPEN, OnBtnSpOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeChkPage1 message handlers

BOOL CTimeChkPage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	char tbuf[128];
	sprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	if( gItem.GetValue("SPECIALDAYCHK", tbuf) )
	{
		if( atoi(tbuf) )
		{
			this->m_chkspeday.SetCheck(1);
			this->m_edsgoto.EnableWindow(TRUE);
			if( gItem.GetValue("ICHK_GO", tbuf) )
				this->m_edsgoto.SetWindowText(tbuf);
			if( gItem.GetValue("SPECIALDAYCHK_FNAME", tbuf ))
				this->m_edSFname.SetWindowText(tbuf);
			else
				this->m_edSFname.SetWindowText("blocktime.ini");
		}
		else
		{
			this->m_edsgoto.EnableWindow(FALSE);
			this->m_edSFname.EnableWindow(FALSE);
		}
	}
	else
	{
		this->m_edsgoto.EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTimeChkPage1::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	char tbuf[128];
	gItem.DelItem("SPECIALDAYCHK");
	gItem.DelItem("ICHK_GO");
	gItem.DelItem("SPECIALDAYCHK_FNAME");
	if( this->m_chkspeday.GetCheck() )
	{
		gItem.AddItem("SPECIALDAYCHK", "1");
		if( this->m_edsgoto.GetWindowText(tbuf, 128) )
			gItem.AddItem("ICHK_GO", tbuf);
		if( this->m_edSFname.GetWindowText(tbuf, 128) )
			gItem.AddItem("SPECIALDAYCHK_FNAME", tbuf);
	}
	CPropertyPage::OnOK();
}

BOOL CTimeChkPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CTimeChkPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

void CTimeChkPage1::OnChkspecialday() 
{
	if( this->m_chkspeday.GetCheck() )
	{
		this->m_edsgoto.EnableWindow();
		this->m_edSFname.EnableWindow();
	}
	else
	{
		this->m_edsgoto.EnableWindow(FALSE);
		this->m_edSFname.EnableWindow(FALSE);
	}
}

void CTimeChkPage1::OnBtnSpOpen() 
{
	CFileDialog dlg(FALSE, "ini", "*.ini", NULL, "ini");
	if( dlg.DoModal() == IDOK )
	{
		CString csFileName = dlg.GetFileName();
		this->m_edSFname.SetWindowText(csFileName);
	}
}
