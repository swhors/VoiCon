// properpage7.cpp : implementation file
//

#include "stdafx.h"
#include "bisconmanager.h"
#include "properpage7.h"

//#include "ioctrl.h"
//#include "ValInterface.h"
//#include "FileInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProperPage7 property page

IMPLEMENT_DYNCREATE(CProperPage7, CPropertyPage)

CProperPage7::CProperPage7() : CPropertyPage(CProperPage7::IDD)
{
	//{{AFX_DATA_INIT(CProperPage7)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProperPage7::~CProperPage7()
{
}

void CProperPage7::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProperPage7)
	DDX_Control(pDX, IDC_CHK_H245TUNNELING, m_chk_h245Tunneling);
	DDX_Control(pDX, IDC_CHK_FASTSTAR, m_chk_fastStart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProperPage7, CPropertyPage)
	//{{AFX_MSG_MAP(CProperPage7)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProperPage7 message handlers

void CProperPage7::OnOK() 
{
#if 1
	int iVal=0;
	char tbuf[32];
	UpdateData(TRUE );
	wsprintf(tbuf, "%d", m_chk_fastStart.GetCheck());
	::WritePrivateProfileString("BISCON_Q931", "FastStart", tbuf, "./biscon.ini");
	wsprintf(tbuf, "%d", m_chk_h245Tunneling.GetCheck());
	::WritePrivateProfileString("BISCON_Q931", "H245Tunneling", tbuf, "./biscon.ini");
#else
	int iVal=0;
	char tbuf[32];
	//if( this->m_chk_fastStart.GetCheck())
	UpdateData(TRUE );
	if( this->m_chk_fastStart.GetCheck())
		iVal = 1;
	wsprintf(tbuf, "%d", iVal );
	ConfWrite("FastStart", NULL, &iVal, FALSE);
	iVal = 0;
	if( this->m_chk_h245Tunneling.GetCheck())
		iVal = 1;
	ConfWrite("H245Tunneling", NULL, &iVal, FALSE);
#endif
	CPropertyPage::OnOK();
}

BOOL CProperPage7::OnInitDialog() 
{
#if 0
	int iVal=0;
	CPropertyPage::OnInitDialog();

	ConfRead("FastStart", NULL, &iVal, FALSE);
	if( iVal )
		this->m_chk_fastStart.SetCheck(1);
	iVal=0;
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0, "./biscon.ini");
	if( iVal )
		this->m_chk_h245Tunneling.SetCheck(1);
	UpdateData(FALSE);
#else
	int iVal=0;
	CPropertyPage::OnInitDialog();
	this->m_chk_fastStart.SetCheck(::GetPrivateProfileInt("BISCON_Q931", "FastStart", 0, "./biscon.ini"));
	this->m_chk_h245Tunneling.SetCheck(GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0, "./biscon.ini"));
	UpdateData(FALSE);
#endif
	return TRUE;
}
