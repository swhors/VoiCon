// ProperPageQ931.cpp : implementation file
//

#include "stdafx.h"
#include "bisconmanager.h"
#include "ProperPageQ931.h"

extern CBISConManagerApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProperPageQ931 property page

IMPLEMENT_DYNCREATE(CProperPageQ931, CPropertyPage)

CProperPageQ931::CProperPageQ931() : CPropertyPage(CProperPageQ931::IDD)
{
	//{{AFX_DATA_INIT(CProperPageQ931)
	m_ConnectTimeout = 0;
	m_CallSignallingPort = 0;
	m_ResponseTimeout = 0;
	//}}AFX_DATA_INIT
}

CProperPageQ931::~CProperPageQ931()
{
}

void CProperPageQ931::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProperPageQ931)
	DDX_Control(pDX, IDC_CHK_FASTSTAR, m_chk_fastStart);
	DDX_Control(pDX, IDC_CHK_H245TUNNELING, m_chk_h245Tunneling);
	DDX_Text(pDX, IDC_ConnectTimeout, m_ConnectTimeout);
	DDX_Text(pDX, IDC_Q931CallSinallingPort, m_CallSignallingPort);
	DDX_Text(pDX, IDC_ResponseTimeout, m_ResponseTimeout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProperPageQ931, CPropertyPage)
	//{{AFX_MSG_MAP(CProperPageQ931)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProperPageQ931 message handlers

void CProperPageQ931::OnOK() 
{
	char tbuf[64];
	CBISConManagerApp * pApp = (CBISConManagerApp *)&theApp;
	UpdateData(TRUE );
	wsprintf(tbuf, "%d", this->m_ResponseTimeout);
	::WritePrivateProfileString("BISCON_Q931", "Q931_ResponseTimeOut", tbuf, "./biscon.ini");
	wsprintf(tbuf, "%d", this->m_ConnectTimeout);
	::WritePrivateProfileString("BISCON_Q931", "Q931_ConnectTimeOut", tbuf, "./biscon.ini");
	wsprintf(tbuf, "%d", this->m_CallSignallingPort);
	::WritePrivateProfileString("BISCON_Q931", "Q931_CallSignalingPort", tbuf, "./biscon.ini");
	wsprintf(tbuf, "%d", m_chk_fastStart.GetCheck());
	::WritePrivateProfileString("BISCON_Q931", "FastStart", tbuf, "./biscon.ini");
	wsprintf(tbuf, "%d", m_chk_h245Tunneling.GetCheck());
	::WritePrivateProfileString("BISCON_Q931", "Q931_H245Tunneling", tbuf, "./biscon.ini");
	CPropertyPage::OnOK();
}

BOOL CProperPageQ931::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	UpdateData(TRUE );
	this->m_ResponseTimeout = ::GetPrivateProfileInt("BISCON_Q931", "Q931_ResponseTimeOut", 4, "./biscon.ini");
	this->m_ConnectTimeout = ::GetPrivateProfileInt("BISCON_Q931", "Q931_ConnectTimeOut", 100, "./biscon.ini");
	this->m_CallSignallingPort = ::GetPrivateProfileInt("BISCON_Q931", "Q931_CallSignalingPort", 1720, "./biscon.ini");
	theApp.m_pIVR_System.m_Q931Port = this->m_CallSignallingPort;
	this->m_chk_fastStart.SetCheck(::GetPrivateProfileInt("BISCON_Q931", "FastStart", 0, "./biscon.ini"));
	this->m_chk_h245Tunneling.SetCheck(GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0, "./biscon.ini"));
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
