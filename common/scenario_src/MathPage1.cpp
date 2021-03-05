// MathPage1.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "MathPage1.h"

#include "Item.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMathPage1 property page

IMPLEMENT_DYNCREATE(CMathPage1, CPropertyPage)

CMathPage1::CMathPage1() : CPropertyPage(CMathPage1::IDD)
{
	//{{AFX_DATA_INIT(CMathPage1)
	//}}AFX_DATA_INIT
}

CMathPage1::~CMathPage1()
{
}

void CMathPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMathPage1)

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMathPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CMathPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMathPage1 message handlers

BOOL CMathPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMathPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CMathPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

void CMathPage1::OnOK() 
{
	gItem.DelItem();
	
	CPropertyPage::OnOK();
}
