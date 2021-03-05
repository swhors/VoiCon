// LogicalConditionPage1.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "LogicalConditionPage1.h"

#include "Item.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogicalConditionPage1 property page

IMPLEMENT_DYNCREATE(CLogicalConditionPage1, CPropertyPage)

CLogicalConditionPage1::CLogicalConditionPage1() : CPropertyPage(CLogicalConditionPage1::IDD)
{
	//{{AFX_DATA_INIT(CLogicalConditionPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CLogicalConditionPage1::~CLogicalConditionPage1()
{
}

void CLogicalConditionPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogicalConditionPage1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogicalConditionPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CLogicalConditionPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogicalConditionPage1 message handlers

BOOL CLogicalConditionPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogicalConditionPage1::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	gItem.DelItem();	
	CPropertyPage::OnOK();
}

BOOL CLogicalConditionPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CLogicalConditionPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}
