#if !defined(AFX_TESTWEB1CTL_H__99C14BAF_B6DC_4253_9837_0EBA0DF58EF0__INCLUDED_)
#define AFX_TESTWEB1CTL_H__99C14BAF_B6DC_4253_9837_0EBA0DF58EF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WebFormView.h"
#include "ItemEntityContainer.h"
#include "ScenarioItem.h"

// TestWeb1Ctl.h : Declaration of the CTestWeb1Ctrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CTestWeb1Ctrl : See TestWeb1Ctl.cpp for implementation.

class CTestWeb1Ctrl : public COleControl
{
	DECLARE_DYNCREATE(CTestWeb1Ctrl)
	
// Constructor
public:
	CTestWeb1Ctrl();
	CWebFormView *m_pTestFormView;
	
	CScenarioItem cItem;
	int ItemCount;

// Operations
public:
	CItemEntityContainer* GetData() { return &m_objs; }
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestWeb1Ctrl)
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CTestWeb1Ctrl();

	DECLARE_OLECREATE_EX(CTestWeb1Ctrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CTestWeb1Ctrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CTestWeb1Ctrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CTestWeb1Ctrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CTestWeb1Ctrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CTestWeb1Ctrl)
	afx_msg void ResetEditor();
	afx_msg void ItemCondition();
	afx_msg void ItemHub();
	afx_msg void ItemAndOr();
	afx_msg void ItemLoop();
	afx_msg void ItemMath();
	afx_msg void ItemIS();
	afx_msg void ItemRetrial();
	afx_msg void ItemPoint();
	afx_msg void ItemScePage();
	afx_msg void ItemInit();
	afx_msg void ItemPlay();
	afx_msg void ItemInput();
	afx_msg void ItemPlug();
	afx_msg void ItemTime();
	afx_msg void SaveEditor();
	afx_msg void LoadEditor();
	afx_msg void ItemPhone();
	afx_msg void ItemVoiceRec();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CTestWeb1Ctrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
private:
	CItemEntityContainer	m_objs;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTWEB1CTL_H__99C14BAF_B6DC_4253_9837_0EBA0DF58EF0__INCLUDED)
