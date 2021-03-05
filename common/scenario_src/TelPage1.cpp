// TelPage1.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "TelPage1.h"
#include "Item.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTelPage1 property page

IMPLEMENT_DYNCREATE(CTelPage1, CPropertyPage)

CTelPage1::CTelPage1() : CPropertyPage(CTelPage1::IDD)
{
	//{{AFX_DATA_INIT(CTelPage1)
	//}}AFX_DATA_INIT
}

CTelPage1::~CTelPage1()
{
}

void CTelPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTelPage1)
	DDX_Control(pDX, IDC_ED_DEFCALLDURATION, m_edDefCallDuration);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTelPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CTelPage1)
	ON_EN_CHANGE(IDC_ED_DEFCALLDURATION, OnChangeEdDefcallduration)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelPage1 message handlers

BOOL CTelPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	if( gItem.GetValue("DEFCALLDURATION", tbuf) )
	{
		this->m_edDefCallDuration.SetWindowText(tbuf);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTelPage1::OnOK() 
{
	char tbuf[128];
	//gItem.DelItem();
	gItem.DelItem("DEFCALLDURATION");
	if( this->m_edDefCallDuration.GetWindowText(tbuf, 128) )
	{
		gItem.AddItem("DEFCALLDURATION", tbuf);
	}
	CPropertyPage::OnOK();
}

BOOL CTelPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CTelPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

void CTelPage1::OnChangeEdDefcallduration() 
{
	char tbuf[128];
	if( this->m_edDefCallDuration.GetWindowText(tbuf, 128) )
	{
		if( IsCharAlpha(tbuf[0]) )
		{
			AfxMessageBox("Only number is allowed.");
			this->m_edDefCallDuration.SetWindowText("");
		}
	}
}
