#if !defined(AFX_TESTWEB1PPG_H__3B25C5CF_7B76_431E_8BC4_7F6DA4C7FF06__INCLUDED_)
#define AFX_TESTWEB1PPG_H__3B25C5CF_7B76_431E_8BC4_7F6DA4C7FF06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TestWeb1Ppg.h : Declaration of the CTestWeb1PropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CTestWeb1PropPage : See TestWeb1Ppg.cpp.cpp for implementation.

class CTestWeb1PropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CTestWeb1PropPage)
	DECLARE_OLECREATE_EX(CTestWeb1PropPage)

// Constructor
public:
	CTestWeb1PropPage();

// Dialog Data
	//{{AFX_DATA(CTestWeb1PropPage)
	enum { IDD = IDD_PROPPAGE_TESTWEB1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CTestWeb1PropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTWEB1PPG_H__3B25C5CF_7B76_431E_8BC4_7F6DA4C7FF06__INCLUDED)
