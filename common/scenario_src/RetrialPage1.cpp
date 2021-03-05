// RetrialPage1.cpp : implementation file
//

//
// dev : 2006/07/12 swhors@naver.com
// 

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "RetrialPage1.h"

#include "typedef.h"

#include "Item.h"

extern CItem	gItem;
#define resourceIDCount		3
DWORD	resourceID[]={IDS_INARG20,IDS_INARG21,IDS_INARG23};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRetrialPage1 property page

IMPLEMENT_DYNCREATE(CRetrialPage1, CPropertyPage)

CRetrialPage1::CRetrialPage1() : CPropertyPage(CRetrialPage1::IDD)
{
	//{{AFX_DATA_INIT(CRetrialPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CRetrialPage1::~CRetrialPage1()
{
}

void CRetrialPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRetrialPage1)
	DDX_Control(pDX, IDC_ED_VALUE, m_edValue);
	DDX_Control(pDX, IDC_COMBO_COMMAND, m_cbCommand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRetrialPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CRetrialPage1)
	ON_CBN_SELCHANGE(IDC_COMBO_COMMAND, OnSelchangeComboCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRetrialPage1 message handlers

BOOL CRetrialPage1::OnInitDialog() 
{
	int		iCurSel;
	char	tbuf[128];
	CPropertyPage::OnInitDialog();
	
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	if( gItem.GetValue("COMMAND", tbuf ) )
	{
		iCurSel = atoi(tbuf);
		this->init_cb(iCurSel);
		if( iCurSel == 2 )
		{
			if( gItem.GetValue("VALUE", tbuf ) )
			{
				this->m_edValue.SetWindowText(tbuf);
			}
		}
	}
	else
	{
		this->init_cb();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRetrialPage1::OnOK() 
{
	int		iCurSel;
	char	tbuf[64];
	gItem.DelItem();
	iCurSel = this->m_cbCommand.GetCurSel();
	if( iCurSel >= 0 )
	{
		wsprintf(tbuf, "%d", iCurSel );
		gItem.AddItem("COMMAND", tbuf);
		if( iCurSel == 2 )
		{
			if( this->m_edValue.GetWindowText(tbuf, 64) )
			{
				gItem.AddItem("INPUT", 0);
				gItem.AddItem("VALUE", tbuf );
			}
		}
	}
	gItem.SetMtype(IDMT_RETRIAL);
	CPropertyPage::OnOK();
}

BOOL CRetrialPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CRetrialPage1::OnCancel() 
{
	CPropertyPage::OnCancel();
}

void CRetrialPage1::init_cb(IN int iSelItem)
{
	char tbuf[64];
	for(int i =0 ; i < resourceIDCount ; i ++ )
	{
		LoadString(AfxGetApp()->m_hInstance, resourceID[i], tbuf, 64);
		this->m_cbCommand.AddString(tbuf);
	}
	if( iSelItem == 2 )
		this->m_edValue.EnableWindow();
	else
		this->m_edValue.EnableWindow(FALSE);
	if( iSelItem >= 0 )
		this->m_cbCommand.SetCurSel(iSelItem);
}

void CRetrialPage1::OnSelchangeComboCommand() 
{
	if( this->m_cbCommand.GetCurSel() == 2 )
		this->m_edValue.EnableWindow();
	else
		this->m_edValue.EnableWindow(FALSE);
}
