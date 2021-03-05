// BISConManagerDoc.cpp : implementation of the CBISConManagerDoc class
//

#include "stdafx.h"
#include "BISConManager.h"

#include "BISConManagerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBISConManagerDoc

IMPLEMENT_DYNCREATE(CBISConManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CBISConManagerDoc, CDocument)
	//{{AFX_MSG_MAP(CBISConManagerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBISConManagerDoc construction/destruction

CBISConManagerDoc::CBISConManagerDoc()
{
	// TODO: add one-time construction code here

}

CBISConManagerDoc::~CBISConManagerDoc()
{
}

BOOL CBISConManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	this->SetTitle("Line Infomation");

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CBISConManagerDoc serialization

void CBISConManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBISConManagerDoc diagnostics

#ifdef _DEBUG
void CBISConManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBISConManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBISConManagerDoc commands
