// PagePage1.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif
#include "PagePage1.h"

#include "Item.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPagePage1 property page

IMPLEMENT_DYNCREATE(CPagePage1, CPropertyPage)

CPagePage1::CPagePage1() : CPropertyPage(CPagePage1::IDD)
{
	//{{AFX_DATA_INIT(CPagePage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPagePage1::~CPagePage1()
{
}

void CPagePage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePage1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePage1, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePage1 message handlers

BOOL CPagePage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	char tbuf[128];
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPagePage1::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	gItem.DelItem();
	CPropertyPage::OnOK();
}
