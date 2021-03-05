// BISConManagerDoc.h : interface of the CBISConManagerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BISCONMANAGERDOC_H__B6210D9B_2AC2_4437_BA09_36609AA91972__INCLUDED_)
#define AFX_BISCONMANAGERDOC_H__B6210D9B_2AC2_4437_BA09_36609AA91972__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBISConManagerDoc : public CDocument
{
protected: // create from serialization only
	CBISConManagerDoc();
	DECLARE_DYNCREATE(CBISConManagerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBISConManagerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBISConManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBISConManagerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BISCONMANAGERDOC_H__B6210D9B_2AC2_4437_BA09_36609AA91972__INCLUDED_)
