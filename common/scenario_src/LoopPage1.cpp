// LoopPage1.cpp : implementation file
//

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "scenarioeditor.h"
#endif
#include "LoopPage1.h"

#include "Item.h"

extern CItem	gItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD dwResID[]={IDC_RD11,IDC_RD12,IDC_RD13,IDC_RD14,IDC_RD15};

/////////////////////////////////////////////////////////////////////////////
// CLoopPage1 property page

IMPLEMENT_DYNCREATE(CLoopPage1, CPropertyPage)

CLoopPage1::CLoopPage1() : CPropertyPage(CLoopPage1::IDD)
{
	//{{AFX_DATA_INIT(CLoopPage1)
	//}}AFX_DATA_INIT
}

CLoopPage1::~CLoopPage1()
{
}

void CLoopPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoopPage1)
	DDX_Control(pDX, IDC_CHK_CALLER, m_chkCaller);
	DDX_Control(pDX, IDC_CHK_CALLEE, m_chkCallee);
	DDX_Control(pDX, IDC_RD15, m_rd15);
	DDX_Control(pDX, IDC_RD14, m_rd14);
	DDX_Control(pDX, IDC_RD13, m_rd13);
	DDX_Control(pDX, IDC_RD12, m_rd12);
	DDX_Control(pDX, IDC_RD11, m_rd11);
	DDX_Control(pDX, IDC_ED_NUM, m_edNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoopPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CLoopPage1)
	ON_BN_CLICKED(IDC_RD13, OnRd13)
	ON_BN_CLICKED(IDC_RD12, OnRd12)
	ON_BN_CLICKED(IDC_RD11, OnRd11)
	ON_BN_CLICKED(IDC_RD14, OnRd14)
	ON_BN_CLICKED(IDC_RD15, OnRd15)
	ON_EN_CHANGE(IDC_ED_NUM, OnChangeEdNum)
	ON_BN_CLICKED(IDC_CHK_CALLER, OnChkCaller)
	ON_BN_CLICKED(IDC_CHK_CALLEE, OnChkCallee)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoopPage1 message handlers

void CLoopPage1::OnRd13() 
{
	// TODO: Add your control notification handler code here
	EnableRdButton(IDC_RD13);
}

void CLoopPage1::OnRd12() 
{
	// TODO: Add your control notification handler code here
	EnableRdButton(IDC_RD12);
}

void CLoopPage1::OnRd11() 
{
	// TODO: Add your control notification handler code here
	EnableRdButton(IDC_RD11);
}

void CLoopPage1::OnRd14() 
{
	// TODO: Add your control notification handler code here
	EnableRdButton(IDC_RD14);
}

void CLoopPage1::OnRd15()
{
	// TODO: Add your control notification handler code here
	EnableRdButton(IDC_RD15);
}

void CLoopPage1::EnableRdButton(unsigned int iID)
{
	BOOL bEnable=TRUE;
	this->m_rd11.SetCheck(0);
	this->m_rd12.SetCheck(0);
	this->m_rd13.SetCheck(0);
	this->m_rd14.SetCheck(0);
	this->m_rd15.SetCheck(0);
	this->m_edNum.EnableWindow(FALSE);
	switch(iID)
	{
	case IDC_RD11:
		this->m_rd11.SetCheck(1);
		break;
	case IDC_RD12:
		this->m_rd12.SetCheck(1);
		break;
	case IDC_RD13: // out bound call drop
		this->m_rd13.SetCheck(1);
		bEnable = FALSE;
		break;
	case IDC_RD14:
		this->m_rd14.SetCheck(1);
		this->m_edNum.EnableWindow();
		break;
	case IDC_RD15: // time out.
		this->m_rd15.SetCheck(1);
		this->m_edNum.EnableWindow();
		bEnable = FALSE;
		break;
	}
	if( bEnable == FALSE )
	{
		this->m_chkCaller.SetCheck(0);
		this->m_chkCallee.SetCheck(0);
	}
	this->m_chkCaller.EnableWindow(bEnable);
	this->m_chkCallee.EnableWindow(bEnable);
}

BOOL CLoopPage1::OnInitDialog() 
{
	char tbuf[128];
	CPropertyPage::OnInitDialog();
	
	wsprintf(tbuf, "%d", gItem.GetCount());
	SetDlgItemText(IDC_ED_ID, tbuf);
	
	if( gItem.GetValue("STOPUINPUT", tbuf) )
	{
		//	//0~9:0~9 10:* 11:# 12:any 13:none
		int iStop = atoi(tbuf);
		switch(iStop)
		{
		case 10: // star
			EnableRdButton(IDC_RD12);
			break;
		case 11: // sharp
			EnableRdButton();
			break;
		case 12:// any
			//EnableRdButton();
			break;
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			EnableRdButton(IDC_RD14);
			this->m_edNum.SetWindowText(tbuf);
			break;
		case 13: //
			break;
		case 14: // outbound call drop
			EnableRdButton(IDC_RD13);
			break;
		default:
			{
				if( iStop > 14 && iStop <= 360 )
				{
					EnableRdButton(IDC_RD15);
					this->m_edNum.SetWindowText(tbuf);
				}
				else
					EnableRdButton();
			}
			break;
		}
	}
	else
	{
		EnableRdButton();
	}
	if( gItem.GetValue("DIRECTION", tbuf ))
	{
		int iVal = atoi(tbuf);
		if(iVal ==0)
		{
			this->m_chkCaller.SetCheck(1);
		}
		else if( iVal == 1)
		{
			this->m_chkCallee.SetCheck(1);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoopPage1::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	char tbuf[128];
	gItem.DelItem();
	DWORD dwSel = this->GetCheckedRadioButton(IDC_RD11, IDC_RD15);
	switch( dwSel )
	{
	case IDC_RD11:// sharp
		gItem.AddItem("STOPUINPUT", "11");
		break;
	case IDC_RD12:// star
		gItem.AddItem("STOPUINPUT", "10");
		break;
	case IDC_RD13:// out bounf Call Drop
		gItem.AddItem("STOPUINPUT", "14");
		break;
	case IDC_RD14:
	case IDC_RD15:
		{
			if( this->m_edNum.GetWindowText(tbuf, 128) )
				gItem.AddItem("STOPUINPUT", tbuf);
		}
		break;
	}
	if( this->m_chkCallee.GetCheck() )
	{
		gItem.AddItem("DIRECTION", "1");
	}
	else if( this->m_chkCaller.GetCheck() )
	{
		gItem.AddItem("DIRECTION", "0");
	}
	CPropertyPage::OnOK();
}

BOOL CLoopPage1::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CLoopPage1::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

void CLoopPage1::OnChangeEdNum() 
{
	char tbuf[128];
	if( this->m_edNum.GetWindowText(tbuf, 128) )
	{
		int num = atoi(tbuf);
		DWORD dwSel = this->GetCheckedRadioButton(IDC_RD11, IDC_RD15);
		if( dwSel == IDC_RD14)
		{
			if( 0> num || num > 9 )
			{
				AfxMessageBox("Out of Range.(0~9)");
				this->m_edNum.SetWindowText("");
			}
		}
		else if( dwSel == IDC_RD15 )
		{
			if( 0> num || num > 360 )
			{
				AfxMessageBox("Out of Range.(15~360)");
				this->m_edNum.SetWindowText("");
			}
		}
	}
}

void CLoopPage1::OnChkCaller() 
{
	if( this->m_chkCaller.GetCheck() )
		this->m_chkCallee.SetCheck(0);
}

void CLoopPage1::OnChkCallee() 
{
	if( this->m_chkCallee.GetCheck() )
		this->m_chkCaller.SetCheck(0);
	
}
