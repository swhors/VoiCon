// InputPage1.cpp : implementation file
//

//
// dev :
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif // WEBAPP
#include "InputPage1.h"

#include "Item.h"

extern CItem	gItem;

#define INMAXID	7

DWORD	inStrID1[] = {IDS_OUTARG0,IDS_OUTARG1,IDS_OUTARG2,IDS_OUTARG3,IDS_OUTARG4,IDS_OUTARG11,IDS_OUTARG12};
DWORD	btnID[]={IDC_RD2,IDC_RD3,IDC_RD4,IDC_RD5};


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputPage1 property page

IMPLEMENT_DYNCREATE(CInputPage1, CPropertyPage)

CInputPage1::CInputPage1() : CPropertyPage(CInputPage1::IDD)
{
	//{{AFX_DATA_INIT(CInputPage1)
	//}}AFX_DATA_INIT
}

CInputPage1::~CInputPage1()
{
}

void CInputPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPage1)
	DDX_Control(pDX, IDC_COMBO_ARG, m_cb_arg);
	DDX_Control(pDX, IDC_EDBUFLENGTH, m_edBufLength);
//	DDX_Control(pDX, IDC_RD5, m_rd5);
//	DDX_Control(pDX, IDC_RD4, m_rd4);
//	DDX_Control(pDX, IDC_RD3, m_rd3);
//	DDX_Control(pDX, IDC_RD2, m_rd2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CInputPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputPage1 message handlers

//DEL void CInputPage1::OnRd2()
//DEL {
//DEL 
//DEL 	SetCheckedRadionButton(IDC_RD2);
//DEL }

//DEL void CInputPage1::OnRd3()
//DEL {
//DEL 
//DEL 	SetCheckedRadionButton(IDC_RD3);
//DEL }

//DEL void CInputPage1::OnRd4()
//DEL {
//DEL 
//DEL 	SetCheckedRadionButton(IDC_RD4);
//DEL }

//DEL void CInputPage1::OnRd5()
//DEL {
//DEL 
//DEL 	SetCheckedRadionButton(IDC_RD5);
//DEL }

BOOL CInputPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
//	if( gItem.GetValue("SHARP", tbuf) )
//	{
//		// 0-exit 1-return 2-character 3-back
//		switch(atoi(tbuf))
//		{
//		case 0: // exit
//			SetCheckedRadionButton(IDC_RD2);
//			break;
//		case 1: // return
//			SetCheckedRadionButton(IDC_RD3);
//			break;
//		case 2: // character
//			SetCheckedRadionButton(IDC_RD4);
//			break;
//		case 3: // back
//			SetCheckedRadionButton(IDC_RD5);
//			break;
//		}
//	}
//	else
//	{
//		SetCheckedRadionButton(IDC_RD2);
//	}
	if( gItem.GetValue("LENGTH", tbuf ) )
	{
		this->m_edBufLength.SetWindowText(tbuf);
	}
	if( gItem.GetValue("INPUT", tbuf ) )
	{
		init_cb(atoi(tbuf));
	}
	else
	{
		init_cb(0);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//DEL void CInputPage1::SetCheckedRadionButton(unsigned int iID)
//DEL {
//DEL 	m_rd2.SetCheck(0);
//DEL 	m_rd3.SetCheck(0);
//DEL 	m_rd4.SetCheck(0);
//DEL 	m_rd5.SetCheck(0);	
//DEL 	switch(iID )
//DEL 	{
//DEL 	case IDC_RD2:
//DEL 		m_rd2.SetCheck(1);
//DEL 		break;
//DEL 	case IDC_RD3:
//DEL 		m_rd3.SetCheck(1);
//DEL 		break;
//DEL 	case IDC_RD4:
//DEL 		m_rd4.SetCheck(1);
//DEL 		break;
//DEL 	case IDC_RD5:
//DEL 		m_rd5.SetCheck(1);
//DEL 		break;
//DEL 	}
//DEL }

void CInputPage1::OnOK() 
{
	char tbuf[128];
	gItem.DelItem();
//	DWORD iID = this->GetCheckedRadioButton(IDC_RD2,IDC_RD5);
//	switch(iID )
//	{
//	case IDC_RD2:
//		gItem.AddItem("SHARP", "0");
//		break;
//	case IDC_RD3:
//		gItem.AddItem("SHARP", "1");
//		break;
//	case IDC_RD4:
//		gItem.AddItem("SHARP", "2");
//		break;
//	case IDC_RD5:
//		gItem.AddItem("SHARP", "3");
//		break;
//	}
	if( this->m_edBufLength.GetWindowText(tbuf, 128) )
		gItem.AddItem("LENGTH", tbuf);
	if( this->m_cb_arg.GetCurSel() >= 0 && this->m_cb_arg.GetCurSel()<5)
	{
		char tbuf[64];
		wsprintf(tbuf, "%d", this->m_cb_arg.GetCurSel()+5);
		gItem.AddItem("INPUT", tbuf);
	}
	else if( this->m_cb_arg.GetCurSel() == 5 )
	{
		char tbuf[64];
		wsprintf(tbuf, "%d", 1);
		gItem.AddItem("INPUT", tbuf);
	}
	else if( this->m_cb_arg.GetCurSel() == 6 )
	{
		char tbuf[64];
		wsprintf(tbuf, "%d", 11);
		gItem.AddItem("INPUT", tbuf);
	}
	CPropertyPage::OnOK();
}

void CInputPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

BOOL CInputPage1::OnApply() 
{
	return CPropertyPage::OnApply();
}

void CInputPage1::init_cb(int iSelectedItem)
{
	char tbuf[64];
	
	for(int i = 0; i< INMAXID; i++)
	{
		LoadString(AfxGetApp()->m_hInstance, inStrID1[i], tbuf, 64);
		this->m_cb_arg.AddString(tbuf);
	}
	
	if( (iSelectedItem >= 5) && (iSelectedItem < 9) )
	{
		//if( iSelectedItem <4 )
		//	this->m_cb_arg.SetCurSel(iSelectedItem);
		//else
		this->m_cb_arg.SetCurSel(iSelectedItem-5);
	}
	else if( iSelectedItem == 1 )
	{
		this->m_cb_arg.SetCurSel(5);
	}
	else if( iSelectedItem == 11)
	{
		this->m_cb_arg.SetCurSel(6);
	}

}
