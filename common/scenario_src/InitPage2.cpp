// InitPage2.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif
#include "InitPage2.h"

#include "Item.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CItem	gItem;

/////////////////////////////////////////////////////////////////////////////
// CInitPage2 property page

IMPLEMENT_DYNCREATE(CInitPage2, CPropertyPage)

CInitPage2::CInitPage2() : CPropertyPage(CInitPage2::IDD)
{
	//{{AFX_DATA_INIT(CInitPage2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CInitPage2::~CInitPage2()
{
}

void CInitPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPage2)
	DDX_Control(pDX, IDC_LIST_MENT, m_listment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPage2)
	ON_BN_CLICKED(IDC_BTN_MENT, OnBtnMent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPage2 message handlers

void CInitPage2::OnBtnMent() 
{
	CString csBuf;
	if( GetDlgItemText(IDC_ED_MENT, csBuf) > 0 )
	{
		this->m_listment.AddString(csBuf);
		SetDlgItemText(IDC_ED_MENT,"");
	}
}

void CInitPage2::OnOK() 
{
	char tbuf[128];
	char name[32];
	int iCount = this->m_listment.GetCount();
	gItem.DelItem();
	for(int i = 0; i< iCount ; i++)
	{
		if( m_listment.GetText(i, tbuf) )
		{
			wsprintf(name, "MENT%d", i);
			gItem.AddItem(name, tbuf);
		}
	}
	CPropertyPage::OnOK();
}

void CInitPage2::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

BOOL CInitPage2::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CPropertyPage::OnApply();
}

BOOL CInitPage2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	PITEMCOMPONENT pCom=NULL;
	pCom = gItem.GetComponent(pCom);
	while( pCom )
	{
		if( !strncmp(pCom->name, "MENT",4) )
		{
			this->m_listment.AddString(pCom->value);
		}
		pCom = gItem.GetComponent(pCom);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
