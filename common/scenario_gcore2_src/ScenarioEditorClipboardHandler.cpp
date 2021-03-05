/* ==========================================================================
	CScenarioEditorClipboardHandler

	Author :		Johan Rosengren, Abstrakt Mekanik AB

	Date :			2004-05-04

	Purpose :		CScenarioEditorClipboardHandler is a clipboard handler 
					for CNetworkEditor.

	Description :	The class, in addition to the default handling, will 
					take care of assigning new ids to copied objects. It 
					will also copy and paste links.

	Usage :			Add to the editor container using 
					CDiagramEntityContainer::SetClipboardHandler

   ========================================================================*/
#include "stdafx.h"
#include "ScenarioEditorClipboardHandler.h"
#include "ItemEntityContainer.h"
#include "ItemLink.h"
#include "LinkFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CScenarioEditorClipboardHandler::CScenarioEditorClipboardHandler()
{
}

CScenarioEditorClipboardHandler::~CScenarioEditorClipboardHandler()
{
	ClearPaste();
}

void CScenarioEditorClipboardHandler::Copy( CDiagramEntity* obj )
{
	if( obj )
	{
		ClearPaste();
		CDiagramEntity* newobj = obj->Clone();
		newobj->Select( TRUE );
		newobj->MoveRect( 10, 10 );
		newobj->SetName( CLinkFactory::GetID() );
		GetData()->Add( newobj );
	}
}

void CScenarioEditorClipboardHandler::Paste( CDiagramEntityContainer* container )
{
	CDiagramClipboardHandler::Paste( container );
	int max = m_pasteLinks.GetSize();
#ifdef WEBAPP
	CItemEntityContainer* flow = static_cast< CItemEntityContainer* >( container );
#else
	CItemEntityContainer* flow = (CItemEntityContainer*)container;
#endif
	for( int t = 0 ; t < max ; t++ )
#ifdef WEBAPP
		flow->AddLink( (( CItemLink*)m_pasteLinks[ t ])->Clone() );
#else
		flow->AddLink( ( static_cast< CItemLink* >( m_pasteLinks[ t ] ) )->Clone() );
#endif
}

void CScenarioEditorClipboardHandler::CopyAllSelected( CDiagramEntityContainer* container )
{
	int t;
	CDiagramClipboardHandler::CopyAllSelected( container );
#ifdef WEBAPP
	CItemEntityContainer* flow = (CItemEntityContainer*)container;
#else
	CItemEntityContainer* flow = static_cast< CItemEntityContainer* >( container );
#endif
	CObArray* links = flow->GetLinkArray();

	int max = links->GetSize();
	for( t = 0; t < max ; t++ )
	{
#ifdef WEBAPP
		CItemLink* link = (CItemLink*)links->GetAt(t );
#else
		CItemLink* link = static_cast< CItemLink* >( links->GetAt(t ) );
#endif
		m_pasteLinks.Add( link->Clone() );
	}

	CObArray* paste = GetData();
	max = paste->GetSize();

	for( t = 0; t < max ; t++ )
	{
#ifdef WEBAPP
		CDiagramEntity* obj = (CDiagramEntity*)paste->GetAt( t );
#else
		CDiagramEntity* obj = static_cast< CDiagramEntity* >( paste->GetAt( t ) );
#endif
		CString newID = CLinkFactory::GetID();
		
		int maxlinks = m_pasteLinks.GetSize();
		for( int i = 0 ; i < maxlinks ; i++ )
		{
#ifdef WEBAPP
			CItemLink* link = (CItemLink*)m_pasteLinks[ i ];
#else
			CItemLink* link = static_cast< CItemLink* >( m_pasteLinks[ i ] );
#endif
			if( link->from == obj->GetName() )
				link->from = newID;
			if( link->to == obj->GetName() )
				link->to = newID;
		}
		obj->SetName( newID );
	}
}

void CScenarioEditorClipboardHandler::ClearPaste()
{
	CDiagramClipboardHandler::ClearPaste();

	int max = m_pasteLinks.GetSize();
	for( int t = max - 1 ; t >= 0 ; t-- )
		delete m_pasteLinks[t];

	m_pasteLinks.RemoveAll();
}

