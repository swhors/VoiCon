// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Audio.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog


CProgressDlg::CProgressDlg(int &iCount, char * sbuf, CWnd* pParent /*=NULL*/)
:m_iCount(iCount), szFile(sbuf), CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_idc_prog);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

void CProgressDlg::UpBar()
{
	int iVal = m_iCount%100;
	if( iVal != 0)
		m_idc_prog.StepIt();
	else
		m_idc_prog.SetPos(0);
	SetDlgItemText(IDC_FNAME,szFile);
}

BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_idc_prog.SetRange(0, 100);
	m_idc_prog.SetStep(1);
	ShowWindow(TRUE);
	SetForegroundWindow();
	return TRUE;
}

int CProgressDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	return 0;
}