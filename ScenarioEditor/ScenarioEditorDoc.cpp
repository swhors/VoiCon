// ScenarioEditorDoc.cpp : implementation of the CScenarioEditorDoc class
//

#include "stdafx.h"
#include "ScenarioEditor.h"

#include "ScenarioEditorDoc.h"

#include "ScenarioEditorControlFactory.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorDoc

IMPLEMENT_DYNCREATE(CScenarioEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CScenarioEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CScenarioEditorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorDoc construction/destruction

CScenarioEditorDoc::CScenarioEditorDoc()
{
	// TODO: add one-time construction code here
	ItemCount = -1;
}

CScenarioEditorDoc::~CScenarioEditorDoc()
{
}

BOOL CScenarioEditorDoc::OnNewDocument()
{
	this->m_objs.RemoveAll();
	//this->m_objs.ClearLinks();
	this->cItem.DelItem();
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorDoc serialization

void CScenarioEditorDoc::Serialize(CArchive& ar)
{
	CFile *cFile = ar.GetFile();
	if (ar.IsStoring())
	{
		if( !this->cItem.HaveTop() )
		{
			AfxMessageBox("Not have TOP Item:[init]");
			return;
		}
		ar.WriteString("1\r\n");
		ar.WriteString( m_objs.GetString() + _T( "\r\n" ) );
		int count = 0;
		CDiagramEntity* obj;
		while( ( obj = m_objs.GetAt( count++ ) ) )
			ar.WriteString( obj->GetString() + _T( "\r\n" ) );

		int max = m_objs.GetLinks();
		for( int t = 0 ; t < max ; t++ )
		{
			CItemLink* link = m_objs.GetLinkAt( t );
			if( link )
				ar.WriteString( link->GetString() + _T( "\r\n" ) );
		}
		m_objs.SetModified( FALSE );
		CString csTitle = cFile->GetFileTitle();
		csTitle.Replace(".sce","");
		csTitle.Replace(".SCE","");
		this->cItem.WriteToFile(csTitle);
	}
	else
	{
		int bFind = 0;
		m_objs.Clear();
		CString str;
		ar.ReadString(str);
		if( !str.Compare("1\r"))
			bFind = 1;
		if( !str.Compare("1"))
			bFind = 1;
		if( !str.Compare("1\r\n"))
			bFind = 1;
		if( !bFind )
			return;
		while(ar.ReadString( str ) )
		{
			if( !m_objs.FromString( str ) )
			{
				CDiagramEntity* obj = CScenarioEditorControlFactory::CreateFromString( str );
				if( obj )
				{
					obj->SetDocument(this);
					m_objs.Add( obj );
				}
				else
				{
					CItemLink* link = new CItemLink;
					if( link->FromString( str ) )
						m_objs.AddLink( link );
					else
						delete link;
				}
			}
		}
		m_objs.SetModified( FALSE );
		CString csTitle = cFile->GetFileTitle();
		csTitle.Replace(".sce","");
		csTitle.Replace(".SCE","");
		this->ItemCount = this->cItem.ReadFromFile(csTitle);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorDoc diagnostics

#ifdef _DEBUG
void CScenarioEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CScenarioEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditorDoc commands
 

void CScenarioEditorDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDocument::OnCloseDocument();
}

BOOL CScenarioEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	//if( this->m_objs )
	//	delete this->m_objs;
	//this->m_objs = NULL;
	this->m_objs.RemoveAll();
	//this->m_objs.ClearLinks();
	this->cItem.DelItem();
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	
	return TRUE;
}
