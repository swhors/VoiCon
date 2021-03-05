// properpagecodec.cpp : implementation file
//

#include "stdafx.h"
#include "bisconmanager.h"
#include "properpagecodec.h"

int		iChkAudioCodec[4];

/////////////////////////////////////////////////////////////////////////////
// CProperPageCodec property page

IMPLEMENT_DYNCREATE(CProperPageCodec, CPropertyPage)

CProperPageCodec::CProperPageCodec() : CPropertyPage(CProperPageCodec::IDD)
{
	//{{AFX_DATA_INIT(CProperPageCodec)
	m_RtpPort = 0;
	m_audioRecord = FALSE;
	m_rearrangedRTP = FALSE;
	m_mentInterval = 0;
	//}}AFX_DATA_INIT
}

CProperPageCodec::~CProperPageCodec()
{
}

void CProperPageCodec::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProperPageCodec)
	DDX_Control(pDX, IDC_DTMFTIMEOUT, m_ctUserInputReady);
	DDX_Control(pDX, IDC_DTMFINITTIMEOUT, m_ctFirstUserInputReady);
	DDX_Control(pDX, IDC_CHK3, m_chk3);
	DDX_Control(pDX, IDC_CHK2, m_chk2);
	DDX_Control(pDX, IDC_CHK1, m_chk1);
	DDX_Control(pDX, IDC_CBAUDIO3, m_ctCBAudio3);
	DDX_Control(pDX, IDC_CBAUDIO2, m_ctCBAudio2);
	DDX_Control(pDX, IDC_CBAUDIO1, m_ctCBAudio1);
	DDX_Control(pDX, IDC_CHKDTMFENABLE, m_ctChkDTMFEnable);
	DDX_Control(pDX, IDC_CHKSILENCESUPRESSION, m_ctChkSilenceSupression);
	DDX_Control(pDX, IDC_EDAUDIOFRAME, m_ctAudioFrame);
	DDX_Control(pDX, IDC_CBDTMFTYPE, m_ctCbDTMFType);
	DDX_Text(pDX, IDC_RTPPort, m_RtpPort);
	DDX_Check(pDX, IDC_CHKAUDIORECORD2, m_audioRecord);
	DDX_Check(pDX, IDC_CHKRTPREARRANGE, m_rearrangedRTP);
	DDX_Text(pDX, IDC_MENTTIMEOUT, m_mentInterval);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProperPageCodec, CPropertyPage)
	//{{AFX_MSG_MAP(CProperPageCodec)
	ON_BN_CLICKED(IDC_CHKDTMFENABLE, OnChkdtmfenable)
	ON_CBN_EDITCHANGE(IDC_CBAUDIO1, OnEditchangeCbaudio1)
	ON_CBN_EDITCHANGE(IDC_CBAUDIO2, OnEditchangeCbaudio2)
	ON_CBN_EDITCHANGE(IDC_CBAUDIO3, OnEditchangeCbaudio3)
	ON_CBN_SELCHANGE(IDC_CBAUDIO1, OnSelchangeCbaudio1)
	ON_CBN_SELCHANGE(IDC_CBAUDIO2, OnSelchangeCbaudio2)
	ON_CBN_SELCHANGE(IDC_CBAUDIO3, OnSelchangeCbaudio3)
	ON_BN_CLICKED(IDC_CHK1, OnChk1)
	ON_BN_CLICKED(IDC_CHK2, OnChk2)
	ON_BN_CLICKED(IDC_CHK3, OnChk3)
	ON_CBN_SELCHANGE(IDC_CBDTMFTYPE, OnSelchangeCbdtmftype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProperPageCodec message handlers

BOOL CProperPageCodec::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

BOOL CProperPageCodec::OnInitDialog() 
{
	int iVal;
	CPropertyPage::OnInitDialog();
	EnableCommonItem();
	SetCBItem(this->m_ctCBAudio1);
	SetCBItem(this->m_ctCBAudio2);
	SetCBItem(this->m_ctCBAudio3);
	SetCBItem(this->m_ctCbDTMFType,1);
	
	for( int i = 0; i < 4; i++)
	{
		iChkAudioCodec[i] = -1;
	}
	iVal = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE1", -1, "./biscon.ini");
	if( iVal >= 0 )
		SetCodecInfo(0,iVal);
	iVal = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE2", -1, "./biscon.ini");
	if( iVal >= 0 )
		SetCodecInfo(1,iVal);
	iVal = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE3", -1, "./biscon.ini");
	if( iVal >= 0 )
		SetCodecInfo(2,iVal);
	//iVal = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE4", -1, "./biscon.ini");
	//if( iVal >= 0 )
	//	SetCodecInfo(3,iVal);
	iVal = ::GetPrivateProfileInt("BISCON_RTP", "DTMF_TYPE", -1, "./biscon.ini");
	if( iVal >= 0 )
	{
		this->m_ctCbDTMFType.EnableWindow();
		this->m_ctCbDTMFType.SetCurSel(iVal);
		this->EnableUinputTimeOutCtl(TRUE);
		this->m_ctChkDTMFEnable.SetCheck(1);
		char tbuf[32];		
		iVal = GetPrivateProfileInt("BISCON_RTP", "InputInitWaitTime", 60, "./biscon.ini");
		wsprintf(tbuf,"%d", iVal);
		m_ctUserInputReady.SetWindowText(tbuf);
		iVal = GetPrivateProfileInt("BISCON_RTP", "InputWaitTime", 20, "./biscon.ini");
		wsprintf(tbuf,"%d", iVal);
		m_ctFirstUserInputReady.SetWindowText(tbuf);
	}
	else
	{
		this->m_ctChkDTMFEnable.SetCheck(0);
		this->m_ctCbDTMFType.EnableWindow(FALSE);
	}
	ChkG7231Audio(m_ctCBAudio3);

	this->m_RtpPort = ::GetPrivateProfileInt("BISCON_RTP", "RTP_port", 30000, "./biscon.ini");
	if( GetPrivateProfileInt("BISCON_RTP", "RTP_Record", 0,"./biscon.ini") )
		this->m_audioRecord = TRUE;
	else
		this->m_audioRecord = FALSE;
	if( GetPrivateProfileInt("BISCON_RTP", "RTP_ReArrange", 0,"./biscon.ini") )
		this->m_rearrangedRTP = TRUE;
	else
		this->m_rearrangedRTP = FALSE;
	
	this->m_mentInterval = ::GetPrivateProfileInt("BISCON_RTP", "MentInterval", 2, "./biscon.ini");
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProperPageCodec::OnOK() 
{
	char tbuf[64];
	int curSel,iCount=0;
	UpdateData(TRUE );
	WritePrivateProfileString("BISCON_RTP", "CODEC_TYPE1", "-1", "./biscon.ini");
	WritePrivateProfileString("BISCON_RTP", "CODEC_TYPE2", "-1", "./biscon.ini");
	WritePrivateProfileString("BISCON_RTP", "CODEC_TYPE3", "-1", "./biscon.ini");
	WritePrivateProfileString("BISCON_RTP", "CODEC_TYPE4", "-1", "./biscon.ini");
	WritePrivateProfileString("BISCON_RTP", "DTMF_TYPE", "-1", "./biscon.ini");

	WritePrivateProfileString("BISCON_RTP","InputInitWaitTime", "20", "./biscon.ini");
	WritePrivateProfileString("BISCON_RTP","InputWaitTime", "6", "./biscon.ini");

	WritePrivateProfileString("BISCON_RTP", "G7231_maxAl-sduAudioFrames", "1", "./biscon.ini");
	WritePrivateProfileString("BISCON_RTP", "G7231_SilenceSupression", "0", "./biscon.ini");
	curSel = this->m_ctCBAudio1.GetCurSel();
	if( curSel > -1)
		WriteAudioInfo(iCount++, curSel );
	curSel = this->m_ctCBAudio2.GetCurSel();
	if( curSel > -1)
		WriteAudioInfo(iCount++, curSel );
	curSel = this->m_ctCBAudio3.GetCurSel();
	
	if( curSel > -1)
		WriteAudioInfo(iCount++, curSel );
	
	if( this->m_ctChkDTMFEnable.GetCheck() )
	{
		curSel = m_ctCbDTMFType.GetCurSel();
		wsprintf(tbuf, "%d", curSel);
		if( curSel >= 0 )
		{
			WritePrivateProfileString("BISCON_RTP", "DTMF_TYPE", tbuf, "./biscon.ini");
			memset(tbuf, 0, 64);
			m_ctUserInputReady.GetWindowText(tbuf,64);
			WritePrivateProfileString("BISCON_RTP","InputInitWaitTime", tbuf, "./biscon.ini");
			memset(tbuf, 0, 64);
			m_ctFirstUserInputReady.GetWindowText(tbuf, 64);
			WritePrivateProfileString("BISCON_RTP","InputWaitTime", tbuf, "./biscon.ini");
		}
	}	
	wsprintf(tbuf,"%d", this->m_RtpPort);
	::WritePrivateProfileString("BISCON_RTP", "RTP_port", tbuf, "./biscon.ini");
	if( this->m_audioRecord )
		::WritePrivateProfileString("BISCON_RTP", "RTP_Record", "1", "./biscon.ini");
	else
		::WritePrivateProfileString("BISCON_RTP", "RTP_Record", "0", "./biscon.ini");
	if( this->m_rearrangedRTP )
		::WritePrivateProfileString("BISCON_RTP", "RTP_ReArrange", "1", "./biscon.ini");
	else
		::WritePrivateProfileString("BISCON_RTP", "RTP_ReArrange", "0", "./biscon.ini");

	wsprintf(tbuf,"%d", this->m_mentInterval);
	
	::WritePrivateProfileString("BISCON_RTP", "MentInterval", tbuf, "./biscon.ini");


	CPropertyPage::OnOK();
}


void CProperPageCodec::SetCodecInfo(int count,int iCodecType)
{
	switch(count)
	{
	case 0:
		{
			int iVal;
			char tbuf[32];
			this->m_ctCBAudio1.EnableWindow(TRUE);
			this->m_ctCBAudio1.SetCurSel(iCodecType);
			this->m_chk1.SetCheck(1);
			iVal = GetPrivateProfileInt("BISCON_RTP", "G7231_maxAl-sduAudioFrames", 1, "./biscon.ini");
			this->m_ctAudioFrame.EnableWindow(TRUE);
			wsprintf(tbuf, "%d", iVal);
			this->m_ctAudioFrame.SetWindowText(tbuf);
			iVal = GetPrivateProfileInt("BISCON_RTP", "G7231_SilenceSupression", 0, "./biscon.ini");
			this->m_ctChkSilenceSupression.EnableWindow(TRUE);
			this->m_ctChkSilenceSupression.SetCheck(iVal);
			iChkAudioCodec[0] = iCodecType;
		}
		break;
	case 1:
		this->m_ctCBAudio2.EnableWindow(TRUE);
		this->m_ctCBAudio2.SetCurSel(iCodecType);
		this->m_chk2.SetCheck(1);
		iChkAudioCodec[1] = iCodecType;
		break;
	case 2:
		this->m_ctCBAudio3.EnableWindow(TRUE);
		this->m_ctCBAudio3.SetCurSel(iCodecType);
		this->m_chk3.SetCheck(1);
		iChkAudioCodec[2] = iCodecType;
		break;
	//case 3:
	//	this->m_ctCBAudio1.SetCurSel(iCodecType);
	//	break;
	default:
		return;
	}
}

void CProperPageCodec::EnableCommonItem(BOOL bVal)
{
	this->m_ctCbDTMFType.EnableWindow(bVal);
//	this->m_chk1.EnableWindow(bVal);
//	this->m_chk2.EnableWindow(bVal);
//	this->m_chk3.EnableWindow(bVal);
	this->m_ctCBAudio1.EnableWindow(bVal);
	this->m_ctCBAudio2.EnableWindow(bVal);
	this->m_ctCBAudio3.EnableWindow(bVal);
	EnableUinputTimeOutCtl(bVal);
	EnableG7231Item(bVal);
}

void CProperPageCodec::OnChkdtmfenable() 
{
	if( this->m_ctChkDTMFEnable.GetCheck() )
	{
		EnableUinputTimeOutCtl(TRUE);
		this->m_ctCbDTMFType.EnableWindow();
	}
	else
	{
		this->EnableUinputTimeOutCtl(FALSE);
		this->m_ctCbDTMFType.EnableWindow(FALSE);
		this->m_ctCbDTMFType.SetCurSel(-1);
	}
}

void CProperPageCodec::SetCBItem(CComboBox &cb,int type)
{
	if( !type )
	{
		cb.AddString("G7231 64K");
		cb.AddString("G711Ulaw");
		cb.AddString("G711Alaw");
	}
	else
	{
		cb.AddString("DTMF");
		cb.AddString("BasicString");
	}
}

void CProperPageCodec::OnEditchangeCbaudio1() 
{
	iChkAudioCodec[0] = this->m_ctCBAudio1.GetCurSel();
	ChkG7231Audio(m_ctCBAudio1);
}

void CProperPageCodec::OnEditchangeCbaudio2() 
{
	iChkAudioCodec[2] = this->m_ctCBAudio2.GetCurSel();
	ChkG7231Audio(m_ctCBAudio2);
}

void CProperPageCodec::OnEditchangeCbaudio3()
{
	iChkAudioCodec[3] = this->m_ctCBAudio3.GetCurSel();
	ChkG7231Audio(m_ctCBAudio3);
}

void CProperPageCodec::EnableG7231Item(BOOL bVal)
{
	this->m_ctChkSilenceSupression.EnableWindow(bVal);
	this->m_ctAudioFrame.EnableWindow(bVal);
}

void CProperPageCodec::ChkG7231Audio(CComboBox &cb)
{
	int iG7231Count = 0;
	for(int i = 0; i < 4; i ++ )
	{
		if( iChkAudioCodec[i] == 0 )
			iG7231Count ++;

	}
	if( iG7231Count > 0 )
		EnableG7231Item(TRUE);
	else
		EnableG7231Item();
}

void CProperPageCodec::OnSelchangeCbaudio1() 
{
	iChkAudioCodec[0] = this->m_ctCBAudio1.GetCurSel();
	ChkG7231Audio(m_ctCBAudio1);
}

void CProperPageCodec::OnSelchangeCbaudio2() 
{
	iChkAudioCodec[2] = this->m_ctCBAudio2.GetCurSel();
	ChkG7231Audio(m_ctCBAudio2);
}

void CProperPageCodec::OnSelchangeCbaudio3() 
{
	iChkAudioCodec[3] = this->m_ctCBAudio3.GetCurSel();
	ChkG7231Audio(m_ctCBAudio3);
}

void CProperPageCodec::WriteAudioInfo(int Count, int AudioCodec)
{
	char PATH_NAME[64],sVal[32];
	wsprintf(PATH_NAME,"CODEC_TYPE%d", Count+1);
	wsprintf(sVal, "%d", AudioCodec);
	WritePrivateProfileString("BISCON_RTP", PATH_NAME, sVal, "./biscon.ini");
	if( AudioCodec == 0)
	{
		this->m_ctAudioFrame.GetWindowText(sVal, 32);
		WritePrivateProfileString("BISCON_RTP", "G7231_maxAl-sduAudioFrames", sVal, "./biscon.ini");
		if( this->m_ctChkSilenceSupression.GetCheck())
			WritePrivateProfileString("BISCON_RTP", "G7231_SilenceSupression", "1", "./biscon.ini");
		else
			WritePrivateProfileString("BISCON_RTP", "G7231_SilenceSupression", "0", "./biscon.ini");
	}
}

void CProperPageCodec::OnChk1() 
{
	if( this->m_chk1.GetCheck())
	{
		this->m_ctCBAudio1.EnableWindow();
	}
	else
	{
		this->m_ctCBAudio1.EnableWindow(FALSE);
		this->m_ctCBAudio1.SetCurSel(-1);
	}
	OnSelchangeCbaudio1();
}

void CProperPageCodec::OnChk2() 
{
	if( this->m_chk2.GetCheck())
	{
		this->m_ctCBAudio2.EnableWindow();
	}
	else
	{
		this->m_ctCBAudio2.EnableWindow(FALSE);
		this->m_ctCBAudio2.SetCurSel(-1);
	}
	OnSelchangeCbaudio2();
}

void CProperPageCodec::OnChk3() 
{
	if( this->m_chk3.GetCheck())
	{
		this->m_ctCBAudio3.EnableWindow();
	}
	else
	{
		this->m_ctCBAudio3.EnableWindow(FALSE);
		this->m_ctCBAudio3.SetCurSel(-1);
	}
	OnSelchangeCbaudio3();
}

void CProperPageCodec::EnableUinputTimeOutCtl(BOOL bVal)
{
	m_ctFirstUserInputReady.EnableWindow(bVal);
	m_ctUserInputReady.EnableWindow(bVal);
	if( !bVal )
	{
		m_ctFirstUserInputReady.SetWindowText("0");
		m_ctUserInputReady.SetWindowText("0");
	}
	else
	{
		int iVal;
		char tBuf[32];
		iVal = ::GetPrivateProfileInt("BISCON_RTP", "InputInitWaitTime", 20, "./biscon.ini");
		wsprintf(tBuf, "%d", iVal);
		m_ctUserInputReady.SetWindowText(tBuf);
		iVal = ::GetPrivateProfileInt("BISCON_RTP", "InputWaitTime", 6, "./biscon.ini");
		wsprintf(tBuf, "%d", iVal);
		m_ctFirstUserInputReady.SetWindowText("0");

	}
}

void CProperPageCodec::OnSelchangeCbdtmftype() 
{
	//mthis->m_ctCbDTMFType.GetCurSel();
}
