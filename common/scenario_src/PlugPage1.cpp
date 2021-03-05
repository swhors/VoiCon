// PlugPage1.cpp : implementation file
//

#include "stdafx.h"

#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "PlugPage1.h"

#include "Item.h"
#include "typedef.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlugPage1 property page

IMPLEMENT_DYNCREATE(CPlugPage1, CPropertyPage)

CPlugPage1::CPlugPage1() : CPropertyPage(CPlugPage1::IDD)
{
	//{{AFX_DATA_INIT(CPlugPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPlugPage1::~CPlugPage1()
{
}

void CPlugPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlugPage1)
	DDX_Control(pDX, IDC_EDFUNCTION, m_edFunction);
	DDX_Control(pDX, IDC_EDDLLANEM, m_edDllName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlugPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CPlugPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlugPage1 message handlers

void CPlugPage1::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	char tbuf[128];
	gItem.DelItem();
	if( this->m_edDllName.GetWindowText(tbuf, 128) )
		gItem.AddItem("DLLNAME", tbuf);
	if( this->m_edFunction.GetWindowText(tbuf, 128) )
		gItem.AddItem("FUNCTION", tbuf);
	CPropertyPage::OnOK();
	gItem.SetMtype(IDMT_PLUGIN);
}

BOOL CPlugPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CPlugPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

BOOL CPlugPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);

	if( gItem.GetValue("DLLNAME",tbuf) )
		this->m_edDllName.SetWindowText(tbuf);
	if( gItem.GetValue("FUNCTION", tbuf ) )
		this->m_edFunction.SetWindowText(tbuf);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
