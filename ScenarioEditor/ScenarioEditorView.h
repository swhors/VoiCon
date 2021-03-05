// ScenarioEditorView.h : interface of the CScenarioEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENARIOEDITORVIEW_H__19705499_BAAE_4F11_8C1F_BE21627FEEC4__INCLUDED_)
#define AFX_SCENARIOEDITORVIEW_H__19705499_BAAE_4F11_8C1F_BE21627FEEC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CScenarioEditorView : public CView
{
protected: // create from serialization only
	CScenarioEditorView();
	DECLARE_DYNCREATE(CScenarioEditorView)

// Attributes
public:
	CScenarioEditorDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScenarioEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScenarioEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CScenarioEditorView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ScenarioEditorView.cpp
inline CScenarioEditorDoc* CScenarioEditorView::GetDocument()
   { return (CScenarioEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENARIOEDITORVIEW_H__19705499_BAAE_4F11_8C1F_BE21627FEEC4__INCLUDED_)
