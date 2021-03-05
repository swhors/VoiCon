// voicerecpage1.cpp : implementation file
//

#include "stdafx.h"
#if defined WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif
#include "voicerecpage1.h"

#include "Item.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVoiceRecPage1 property page

IMPLEMENT_DYNCREATE(CVoiceRecPage1, CPropertyPage)

CVoiceRecPage1::CVoiceRecPage1() : CPropertyPage(CVoiceRecPage1::IDD)
{
	//{{AFX_DATA_INIT(CVoiceRecPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CVoiceRecPage1::~CVoiceRecPage1()
{
}

void CVoiceRecPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoiceRecPage1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVoiceRecPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CVoiceRecPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceRecPage1 message handlers

void CVoiceRecPage1::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

BOOL CVoiceRecPage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	char tbuf[128];
	sprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
