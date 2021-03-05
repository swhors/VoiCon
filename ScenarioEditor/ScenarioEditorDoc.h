// ScenarioEditorDoc.h : interface of the CScenarioEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENARIOEDITORDOC_H__C60F3EA2_3D07_4BE0_85C1_29A95844C124__INCLUDED_)
#define AFX_SCENARIOEDITORDOC_H__C60F3EA2_3D07_4BE0_85C1_29A95844C124__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ItemEntityContainer.h"

#include "ScenarioItem.h"

class CScenarioEditorDoc : public CDocument
{
protected: // create from serialization only
	CScenarioEditorDoc();
	DECLARE_DYNCREATE(CScenarioEditorDoc)

// Attributes
public:
	CScenarioItem cItem;
	int ItemCount;
 
// Operations
public:
	CItemEntityContainer* GetData() { return &m_objs; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScenarioEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScenarioEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CScenarioEditorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CItemEntityContainer	m_objs;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENARIOEDITORDOC_H__C60F3EA2_3D07_4BE0_85C1_29A95844C124__INCLUDED_)
