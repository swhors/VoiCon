// VoiConLockDlg.cpp : implementation file
//

#include "stdafx.h"
#if 0
#include <cm.h>
#include <cmras.h>
#include <cmsize.h> 
#include <stkutils.h>
#endif
#include "VoiConLock.h"
#include "VoiConLockDlg.h"
#include "CalendarDlg.h"
#include "IvrKeyData.h"
#include "KeyLockCtrlDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVoiConLockDlg dialog

CVoiConLockDlg::CVoiConLockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVoiConLockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVoiConLockDlg)
	m_file_mac = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVoiConLockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoiConLockDlg)
	DDX_Text(pDX, IDC_ED_HOST_MAC, m_file_mac);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVoiConLockDlg, CDialog)
	//{{AFX_MSG_MAP(CVoiConLockDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
	ON_BN_CLICKED(IDC_TIME_BTN, OnTimeBtn)
	ON_BN_CLICKED(IDC_MAC_VERIFY, OnMacVerify)
	ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
	ON_BN_CLICKED(IDC_SET, OnBnClickedSet)
	ON_BN_CLICKED(IDC_BTN_SELF, OnBtnSelf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiConLockDlg message handlers

BOOL CVoiConLockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	FindAutoStorageDevice();
	BYTE hostMAC[32];
	memset(hostMAC, 0, 32);
	GetMacAddr(	(char*)&hostMAC[0] );
	SetDlgItemText( IDC_ED_KEY_MAC2, (char*)&hostMAC[0]);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVoiConLockDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVoiConLockDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVoiConLockDlg::OnGenerate() 
{
	IVRFILELOCK	keyData;
	int iLen=0;
	char tbuff[32];
	memset(&keyData, 0, sizeof( IVRFILELOCK) );
	iLen = GetDlgItemText(IDC_COMPANY,keyData.szCompany, 128);
	if( iLen <= 0){
		MessageBox("Input company name..", "Error", MB_ICONSTOP);
		return;
	}
	iLen = GetDlgItemText(IDC_TIME, keyData.szDueDay, 32);
	if( iLen <= 0){
		MessageBox("Input due..", "Error", MB_ICONSTOP);
		return;
	}
	int iCall = GetDlgItemInt(IDC_USERNUMBER);
	wsprintf(&keyData.szMaxCall[0], "%d", iCall);

#if 0
	HCFG hCfg = ciConstructEx("BISCon.cfg",1024,31);
	if( hCfg )
	{
		ciSetValue(hCfg, "system.maxCalls",FALSE,(INT32)((iCall*2)+10),NULL);
		ciSave(hCfg,"BISCon.cfg");
		ciDestruct(hCfg);
	}
#endif
	iLen = GetDlgItemText(IDC_ED_HOST_MAC, tbuff, 32);
	memcpy( keyData.szHost, tbuff, iLen);
	if( iLen <= 0)
	{
		MessageBox("Input host MAC..", "Error", MB_ICONSTOP);
		return;
	}
	if( !KeyWrite("license.dat", (PBYTE)&keyData, sizeof(IVRFILELOCK), key_auto) )
	{
		AfxMessageBox("Can't create key.");
	}
}

void CVoiConLockDlg::OnTimeBtn() 
{
	CCalendarDlg pDlg;
	pDlg.DoModal();
	CString csTime;
	csTime = pDlg.refDateTime.Format("%Y%m%d");
	SetDlgItemText(IDC_TIME, csTime);
}

void CVoiConLockDlg::OnMacVerify() 
{
	char tbuf[36];
	memset(tbuf, 0, 36);
	if( GetDlgItemText(IDC_ED_HOST_MAC, tbuf,36) > 0 )
	{
		int iRetCode = KeyVerifyWithMac(key_auto);
		switch(iRetCode){
		case -1:
			MessageBox("Same information with this system.","Info", MB_ICONINFORMATION);
			break;
		case 0:
			MessageBox("Don't have system information.","Info", MB_ICONINFORMATION);
			break;
		case 1:
			MessageBox("Same information with this system.","Info", MB_ICONINFORMATION);
			break;
		default:
			MessageBox("Don't know.","Info", MB_ICONINFORMATION);
			break;
		}
	}
	else
	{
		MessageBox("Please! Previously Load License file.", "Notify", MB_ICONINFORMATION);
	}
}

void CVoiConLockDlg::OnBnClickedLoad()
{
	if( !IsUSBEnable() )
	{
		CFileDialog pDlg(TRUE, "dat", "license.dat", OFN_OVERWRITEPROMPT, "License Data|*.dat", NULL);
		if( pDlg.DoModal() == IDOK )
		{
			CString csFile = pDlg.GetPathName();
			if( csFile.GetLength() > 0){
				IVRFILELOCK	keyData;
				if( KeyRead("license.dat", (PBYTE)&keyData, sizeof(IVRFILELOCK), key_file) )
				{
					SetDlgItemText(IDC_COMPANY, keyData.szCompany);
					SetDlgItemText(IDC_TIME, keyData.szDueDay);
					int iCall = atoi( keyData.szMaxCall);
					SetDlgItemInt(IDC_USERNUMBER, iCall);
					SetDlgItemText(IDC_ED_KEY_MAC, keyData.szHost);
					SetDlgItemText(IDC_ED_HOST_MAC, keyData.szHost);
				} else {
					AfxMessageBox("Don't read key information.");
				}
			}
		}
	}
	else
	{
		IVRFILELOCK	keyData;
		{
			char tbuf[32];
			wsprintf(tbuf, "Size Of %d.", sizeof( IVRFILELOCK));
			AfxMessageBox(tbuf);
		}
		if( KeyRead("", (PBYTE)&keyData, sizeof(IVRFILELOCK), key_usb) )
		{
			SetDlgItemText(IDC_COMPANY, keyData.szCompany);
			SetDlgItemText(IDC_TIME, keyData.szDueDay);
			SetDlgItemInt(IDC_USERNUMBER, atoi(keyData.szMaxCall));
			SetDlgItemText(IDC_ED_HOST_MAC, keyData.szHost);
		} else {
			AfxMessageBox("Don't read key information.");
		}
	}
}

void CVoiConLockDlg::OnBnClickedSet()
{
	OnGenerate();
}

void CVoiConLockDlg::OnBtnSelf() 
{
	BYTE hostMAC[32];
	memset(hostMAC, 0, 32);
	GetMacAddr(	(char*)&hostMAC[0]);
	SetDlgItemText( IDC_ED_HOST_MAC, (char*)&hostMAC[0]);
}
