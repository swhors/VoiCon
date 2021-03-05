// IsPage1.cpp : implementation file
//

//
// dev : 2006/07/18 swhors@naver.com
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif
#include "IsPage1.h"
#include "typedef.h"
#include "Item.h"

extern CItem	gItem;

#define ID_MAX	6

DWORD strID1[]={IDS_INARG5,IDS_INARG6,IDS_INARG7,IDS_INARG8,IDS_INARG9,IDS_INARG11 };
DWORD strID2[]={IDS_INARG0,IDS_INARG5,IDS_INARG6,IDS_INARG7,IDS_INARG8,IDS_INARG9,IDS_INARG11 };

int PreviousSel;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIsPage1 property page

IMPLEMENT_DYNCREATE(CIsPage1, CPropertyPage)

CIsPage1::CIsPage1() : CPropertyPage(CIsPage1::IDD)
{
	//{{AFX_DATA_INIT(CIsPage1)
	//}}AFX_DATA_INIT
}

CIsPage1::~CIsPage1()
{
}

void CIsPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIsPage1)
	DDX_Control(pDX, IDC_CHK_01, m_chkUInput);
	DDX_Control(pDX, IDC_CHK_02, m_chkSrcBuffer);
	DDX_Control(pDX, IDC_CB_INBUF, m_cbSrcCtrl);
	DDX_Control(pDX, IDC_COMBO_INPUT, m_cbInput);
	DDX_Control(pDX, IDC_ED_VALUE, m_edValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIsPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CIsPage1)
	ON_CBN_EDITCHANGE(IDC_COMBO_INPUT, OnEditchangeComboInput)
	ON_EN_CHANGE(IDC_ED_VALUE, OnChangeEdValue)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUT, OnSelchangeComboInput)
	ON_BN_CLICKED(IDC_CHK_01, OnChk01)
	ON_BN_CLICKED(IDC_CHK_02, OnChk02)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIsPage1 message handlers

BOOL CIsPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	PreviousSel = 0;
	if( gItem.GetValue("INPUT", tbuf ) )
	{
		init_cb(atoi(tbuf)-4, &this->m_cbInput,0);
	}
	else
	{
		if( gItem.GetMtype() == IDMT_SETLANGUAGE )
			init_cb(6, &this->m_cbInput,0);
		else
			init_cb(-1, &this->m_cbInput,0);
	}
	if( gItem.GetValue("VALUE", tbuf ) )
	{
		this->m_edValue.SetWindowText(tbuf);
		this->m_chkSrcBuffer.SetCheck(0);
		this->m_chkUInput.SetCheck(1);
		this->m_edValue.EnableWindow();
		//this->m_chkSrcBuffer.EnableWindow(FALSE);
	}
	init_cb(-1, &this->m_cbSrcCtrl);

	if( gItem.GetValue("SRCBUF", tbuf) )
	{
		int count = atoi(tbuf)-4;
		this->m_cbSrcCtrl.SetCurSel(count);
		this->m_edValue.EnableWindow(FALSE);
		this->m_chkSrcBuffer.SetCheck(1);
		this->m_chkUInput.SetCheck(0);
	}
	else
	{

		this->m_edValue.EnableWindow(TRUE);
		this->m_chkSrcBuffer.SetCheck(0);
		this->m_chkUInput.SetCheck(1);
		this->m_cbSrcCtrl.EnableWindow(FALSE);
		//this->m_chkSrcBuffer.EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIsPage1::OnOK() 
{
	char tbuf[64];
	int iCurSel = this->m_cbInput.GetCurSel();
	gItem.DelItem();
	if( iCurSel >= 0 && iCurSel < 6)
	{
		wsprintf(tbuf, "%d", iCurSel+4);
		gItem.AddItem("INPUT", tbuf);
		if( this->m_chkUInput.GetCheck() )
		{
			if( this->m_edValue.GetWindowText(tbuf, 64 ) > 0 )
				gItem.AddItem("VALUE", tbuf);
			else
				gItem.AddItem("VALUE", "0");
		}
		if( this->m_chkSrcBuffer.GetCheck() )
		{
			wsprintf(tbuf, "%d", this->m_cbSrcCtrl.GetCurSel()+5);
			gItem.AddItem("SRCBUF", tbuf);
		}
		gItem.SetMtype(IDMT_SETVALUE);		
	}
	else if( iCurSel == 6 )
	{
		gItem.SetMtype(IDMT_SETLANGUAGE);
		if( this->m_edValue.GetWindowText(tbuf, 64 ) > 0 )
			gItem.AddItem("VALUE", tbuf);
		else
			gItem.AddItem("VALUE", "0");
	}
	
	CPropertyPage::OnOK();
}

BOOL CIsPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CIsPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

void CIsPage1::init_cb(int iSelItem, CComboBox *pCombo,int type)
{
	char tbuf[64];
	if( !pCombo )
		return;
	if( type == 1 )
	{
		for(int i = 0; i< ID_MAX; i++)
		{
			LoadString(AfxGetApp()->m_hInstance, strID1[i], tbuf, 64);
			pCombo->AddString(tbuf);
		}
	}
	else if( type == 0 )
	{
		for(int i = 0; i< ID_MAX+1; i++)
		{
			LoadString(AfxGetApp()->m_hInstance, strID2[i], tbuf, 64);
			pCombo->AddString(tbuf);
		}
	}
	if( iSelItem >= 0)
		pCombo->SetCurSel(iSelItem);
}

void CIsPage1::OnEditchangeComboInput() 
{
	if( this->m_cbInput.GetCurSel() == 6 )
		SetValPck(0);
	else
	{
		if( PreviousSel == 6 )
			SetValPck(1);
	}
	PreviousSel = this->m_cbInput.GetCurSel();
}

void CIsPage1::OnChangeEdValue() 
{
	char tbuf[64];
	this->m_edValue.GetWindowText(tbuf, 64);
	if( !::IsCharAlphaNumeric(tbuf[0]) )
	{
		AfxMessageBox("This is not alphanumeric.");
		this->m_edValue.SetWindowText("");
	}
}

void CIsPage1::OnSelchangeComboInput() 
{
//	{
//		this->m_edValue.EnableWindow();
//	}
	if( this->m_cbInput.GetCurSel() == 6 )
		SetValPck(0);
	else
	{
		if( PreviousSel == 6 )
			SetValPck(1);
	}
	PreviousSel = this->m_cbInput.GetCurSel();
}


void CIsPage1::OnChk01() 
{
	if( this->m_chkUInput.GetCheck() )
	{
		this->m_chkSrcBuffer.SetCheck(0);
		this->m_cbSrcCtrl.EnableWindow(FALSE);
		this->m_edValue.EnableWindow(TRUE);
	}
}

void CIsPage1::OnChk02() 
{
	if( this->m_chkSrcBuffer.GetCheck() )
	{
		this->m_chkUInput.SetCheck(0);
		this->m_cbSrcCtrl.EnableWindow(TRUE);
		this->m_edValue.EnableWindow(FALSE);
	}
}

// iOpt == 1 : enable
// iOpt == 0 : disable
void CIsPage1::SetValPck(int iOpt)
{
	this->m_chkUInput.EnableWindow(iOpt);
	this->m_chkSrcBuffer.EnableWindow(iOpt);
	if( !iOpt )
	{
		this->m_cbSrcCtrl.EnableWindow(iOpt);
		this->m_edValue.EnableWindow(TRUE);
	}
	else
		this->m_edValue.EnableWindow(iOpt);
}
