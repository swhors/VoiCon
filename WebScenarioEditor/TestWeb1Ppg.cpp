// TestWeb1Ppg.cpp : Implementation of the CTestWeb1PropPage property page class.

#include "stdafx.h"
#include "TestWeb1.h"
#include "TestWeb1Ppg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CTestWeb1PropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CTestWeb1PropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CTestWeb1PropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CTestWeb1PropPage, "TESTWEB1.TestWeb1PropPage.1",
	0x983e26b6, 0x1b76, 0x4c77, 0xbb, 0xb1, 0xd9, 0x65, 0xe5, 0xa3, 0x13, 0xa7)


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1PropPage::CTestWeb1PropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CTestWeb1PropPage

BOOL CTestWeb1PropPage::CTestWeb1PropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_TESTWEB1_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1PropPage::CTestWeb1PropPage - Constructor

CTestWeb1PropPage::CTestWeb1PropPage() :
	COlePropertyPage(IDD, IDS_TESTWEB1_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CTestWeb1PropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1PropPage::DoDataExchange - Moves data between page and properties

void CTestWeb1PropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CTestWeb1PropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CTestWeb1PropPage message handlers
