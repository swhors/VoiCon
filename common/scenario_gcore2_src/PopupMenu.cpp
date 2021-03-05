// PopupMenu.cpp: implementation of the CPopupMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#include "ItemEntityContainer.h"
#include "TestWeb1Ctl.h"
#else
#include "ScenarioEditor.h"
#include "ScenarioEditorDoc.h"
#endif

#include "PopupMenu.h"
#include "ScenarioEntity.h"
#include "ScenarioEditor1.h"
#include "ScenarioItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPopupMenu::CPopupMenu()
{

}

CPopupMenu::~CPopupMenu()
{

}

CMenu* CPopupMenu::GetPopupMenu( CDiagramEditor* editor )
{
	if( m_menu.m_hMenu == NULL )
		m_menu.LoadMenu( IDR_MENU_POPUP );

	UINT cut = MF_GRAYED;
	UINT link = MF_GRAYED;
	UINT linked = MF_GRAYED;
	UINT dtmf = MF_GRAYED;

	// Link-related
#ifdef WEBAPP
	CScenarioEditor* ed = (CScenarioEditor*)editor;
#else
	CScenarioEditor* ed = dynamic_cast< CScenarioEditor* >( editor );
#endif
	if( ed )
	{
		if( ed->CanLink() )
			link = 0;
		if( ed->IsLinked() )
			linked = 0;
	}
	if( (cut == MF_GRAYED) && (link == MF_GRAYED) && (linked == MF_GRAYED) )
	{
		return NULL;
	}
	
	m_menu.EnableMenuItem( ID_EDIT_CUT, MF_BYCOMMAND | cut );
	m_menu.EnableMenuItem( IDM_ITEM_LINK, MF_BYCOMMAND | link );
	m_menu.EnableMenuItem( IDM_ITEM_UNLINK, MF_BYCOMMAND | linked );

#ifdef WEBAPP
	CItemEntityContainer* objs = (CItemEntityContainer * )ed->GetDiagramEntityContainer();
#else
	CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( ed->GetDiagramEntityContainer() );
#endif
	CScenarioEntity* primary = objs->GetPrimarySelected();
	CScenarioEntity* secondary = objs->GetSecondarySelected();
	if( ed->IsLinked() )
	{
		if( !primary)
			return NULL;
		if( primary->GetItemType() == IDT_HUB )
		{
#ifdef WEBAPP
			if( m_pDoc )
			{
				CScenarioItem * pSce = (CScenarioItem *)&m_pDoc->cItem;
#else
			CScenarioEditorDoc *pDoc = (CScenarioEditorDoc *)ed->GetDocument();
			if( pDoc )
			{
				CScenarioItem * pSce = (CScenarioItem *)&pDoc->cItem;
#endif
				if( pSce )
				{
					if( pSce->GetFalseGo(primary->GetCurID()) != secondary->GetCurID() )
					{
						dtmf = MF_BYCOMMAND|0;
					}
				}
			}
		}
	}
	m_menu.EnableMenuItem( IDM_DTMF_1, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_2, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_3, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_4, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_5, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_6, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_7, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_8, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_9, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_0, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_STAR, dtmf);
	m_menu.EnableMenuItem( IDM_DTMF_SHARP, dtmf);
	return m_menu.GetSubMenu( 0 );
}
