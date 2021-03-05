// properpage6.cpp : implementation file
//

#include "stdafx.h"
#include "BISConManager.h"
#include "properpage6.h"

//#include "ioctrl.h"
//#include "ValInterface.h"
//#include "FileInterface.h"

extern CBISConManagerApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProperPage6 property page

IMPLEMENT_DYNCREATE(CProperPage6, CPropertyPage)

CProperPage6::CProperPage6() : CPropertyPage(CProperPage6::IDD)
{
	//{{AFX_DATA_INIT(CProperPage6)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProperPage6::~CProperPage6()
{
}

void CProperPage6::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProperPage6)
	DDX_Control(pDX, IDC_CHK_AUTO_TERMINAL, m_chk_terminal);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProperPage6, CPropertyPage)
	//{{AFX_MSG_MAP(CProperPage6)
	ON_BN_CLICKED(IDC_CHK_AUTO_TERMINAL, OnChkAutoTerminal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProperPage6 message handlers

LRESULT CProperPage6::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	theApp.IVRMSGProc(message, wParam, lParam);	
	return CPropertyPage::DefWindowProc(message, wParam, lParam);
}

BOOL CProperPage6::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void CProperPage6::OnOK() 
{
	char tbuf[12];
	int iVal = this->m_chk_terminal.GetCheck();
	wsprintf(tbuf, "%d", iVal );
	::WritePrivateProfileString("SystemBase Setup", "AutoStartTerminal", tbuf, "./biscon.ini");
	CPropertyPage::OnOK();
}

BOOL CProperPage6::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
#if 0
	int iVal = ::GetPrivateProfileInt("SystemBase Setup", "AutoStartTerminal", 0, "./biscon.ini");
	if( iVal )
		this->m_chk_terminal.SetCheck(iVal);
	theApp.m_pIVR_System.iAutoStartTerminal = iVal;
#else
	theApp.m_pIVR_System.iAutoStartTerminal = ::GetPrivateProfileInt("SystemBase Setup", "AutoStartTerminal", 0, "./biscon.ini");
	this->m_chk_terminal.SetCheck(theApp.m_pIVR_System.iAutoStartTerminal);
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProperPage6::OnChkAutoTerminal() 
{
	theApp.m_pIVR_System.iAutoStartTerminal = m_chk_terminal.GetCheck();	
}
