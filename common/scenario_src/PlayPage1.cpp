// PlayPage1.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "PlayPage1.h"
#include "Item.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CItem	gItem;

/////////////////////////////////////////////////////////////////////////////
// CPlayPage1 property page

IMPLEMENT_DYNCREATE(CPlayPage1, CPropertyPage)

CPlayPage1::CPlayPage1() : CPropertyPage(CPlayPage1::IDD)
{
	//{{AFX_DATA_INIT(CPlayPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPlayPage1::~CPlayPage1()
{
}

void CPlayPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlayPage1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlayPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CPlayPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayPage1 message handlers

void CPlayPage1::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

BOOL CPlayPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CPropertyPage::OnApply();
}

BOOL CPlayPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlayPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}
