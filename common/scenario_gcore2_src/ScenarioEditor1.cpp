/////////////////////////////////////////////////////////////////////////////
//	CScenarioEditor
//
/////////////////////////////////////////////////////////////////////////////

/* ==========================================================================
	** origianl **
	Author :		(Origianl) Johan Rosengren, Abstrakt Mekanik AB

	Date :			2004-05-01

	Purpose :		CScenarioEditor is a flowchart editor, demonstrating 
					several concepts of CDiagramEditor:	

					1.	Linking objects using a CDiagramEntityContainer-
						derived class to manage data
					2.	Putting CDiagramEditor in a MDI-application, using 
						a shared CDiagramClipboard-derived clipboard handler.
					3.	Exporting the diagram to an enhanced metafile that 
						can be pasted into - for example - Word.
   ========================================================================*/


// ScenarioEditor1.cpp: implementation of the CScenarioEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef WEBAPP
#include "resource.h"
#include "TestWeb1Ctl.h"
#else
#include "ScenarioEditor.h"
#include "ScenarioEditorDoc.h"
#endif

#include "ScenarioEditor1.h"
#include "ScenarioItem.h"
#include "item.h"

#include "ItemEntityContainer.h"
#include "ItemLinkableLineSegment.h"
#include "PopupMenu.h"
#include "typedef.h"
#include "item.h"

extern CItem	gItem;

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditor

CScenarioEditor::CScenarioEditor()
{
	SetBackgroundColor( RGB( 250, 250, 230 ) );
	SetSnapToGrid( TRUE );
	SetRestraints( RESTRAINT_VIRTUAL );
	SetPopupMenu( new CPopupMenu );
	this->m_bClicked = FALSE;
}

CScenarioEditor::~CScenarioEditor()
{
}

BEGIN_MESSAGE_MAP(CScenarioEditor, CDiagramEditor)
	ON_COMMAND(IDM_ITEM_LINK, OnLink)
	ON_COMMAND(IDM_ITEM_UNLINK, OnUnlink)
	ON_COMMAND(IDM_DTMF_1,OnNumber)
	ON_COMMAND(IDM_DTMF_2,OnNumber)
	ON_COMMAND(IDM_DTMF_3,OnNumber)
	ON_COMMAND(IDM_DTMF_4,OnNumber)
	ON_COMMAND(IDM_DTMF_5,OnNumber)
	ON_COMMAND(IDM_DTMF_6,OnNumber)
	ON_COMMAND(IDM_DTMF_7,OnNumber)
	ON_COMMAND(IDM_DTMF_8,OnNumber)
	ON_COMMAND(IDM_DTMF_9,OnNumber)
	ON_COMMAND(IDM_DTMF_0,OnNumber)
	ON_COMMAND(IDM_DTMF_SHARP,OnNumber)
	ON_COMMAND(IDM_DTMF_STAR,OnNumber)
//	ON_COMMAND(IDM_FLOWCHARTEDITOR_LINK_DIRECTION, OnLinkDirection)
//	ON_COMMAND(IDM_ITEM_PROPERTIES, OnLinkProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditor overrides

void CScenarioEditor::DrawGrid( CDC* dc, CRect /*rect*/, double zoom ) const
{

	COLORREF gridcol = GetGridColor();

	dc->SelectStockObject( BLACK_PEN );

	int stepx = GetVirtualSize().cx / GetGridSize().cx;
	int stepy = GetVirtualSize().cy / GetGridSize().cy;

	for( int x = 0 ; x <= stepx ; x++ )
		for( int y = 0; y <= stepy ; y++ )
			dc->SetPixel( round( ( double ) ( GetGridSize().cx * x ) * zoom ), round( ( double ) ( GetGridSize().cy * y ) * zoom ), gridcol );

}

void CScenarioEditor::DrawObjects( CDC* dc, double zoom ) const
{
#ifdef WEBAPP
	CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
	CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
	if( objs )
	{
		CFont font;
		font.CreateFont( -round( 12.0 * zoom ), 0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0, _T( "Courier New" ) );

		int count = GetObjectCount();

		int max = objs->GetLinks();
		int seg = round( ( double ) GetMarkerSize().cx * zoom / 2 );
		POINT pts[ 3 ];

		for( int i = 0 ; i < count ; i++ )
		{
#ifdef WEBAPP
			CDiagramEntity* main = (CDiagramEntity*) objs->GetAt( i );
			CScenarioEntity* obj = (CScenarioEntity*)objs->GetAt( i );
#else
			CDiagramEntity* main = static_cast< CDiagramEntity* >( objs->GetAt( i ) );
			CScenarioEntity* obj = dynamic_cast< CScenarioEntity* >( objs->GetAt( i ) );
#endif
			if( obj )
			{
				for( int t = 0 ; t < max ; t++ )
				{
					CItemLink* link = objs->GetLinkAt( t );
					if( link && link->from == obj->GetName() )
					{

						dc->SelectStockObject( BLACK_PEN );
						dc->SelectStockObject( BLACK_BRUSH );

						CScenarioEntity* to = GetNamedObject( link->to );

						BOOL drawArrow = TRUE;
						CPoint start;
						CPoint end;
						if( obj )
							start = obj->GetLinkPosition( link->fromtype );
						if( to )
							end = to->GetLinkPosition( link->totype );

						start.x = round( ( double ) start.x * zoom );
						start.y = round( ( double ) start.y * zoom );
						end.x = round( ( double ) end.x * zoom );
						end.y = round( ( double ) end.y * zoom );

						switch( link->fromtype )
						{
							case LINK_RIGHT:
								pts[ 1 ].x = end.x - seg * 2;
								pts[ 1 ].y = end.y - seg;
								pts[ 2 ].x = end.x - seg * 2;
								pts[ 2 ].y = end.y + seg;
							break;

							case LINK_LEFT:
								pts[ 1 ].x = end.x + seg * 2;
								pts[ 1 ].y = end.y - seg;
								pts[ 2 ].x = end.x + seg * 2;
								pts[ 2 ].y = end.y + seg;
							break;

							case LINK_TOP:
								pts[ 1 ].x = end.x - seg;
								pts[ 1 ].y = end.y + seg * 2;
								pts[ 2 ].x = end.x + seg;
								pts[ 2 ].y = end.y + seg * 2;
							break;

							case LINK_BOTTOM:
								pts[ 1 ].x = end.x - seg;
								pts[ 1 ].y = end.y - seg * 2;
								pts[ 2 ].x = end.x + seg;
								pts[ 2 ].y = end.y - seg * 2;

							break;

							default:
							{
								switch( link->totype )
								{
									case LINK_RIGHT:
										pts[ 1 ].x = end.x + seg * 2;
										pts[ 1 ].y = end.y - seg;
										pts[ 2 ].x = end.x + seg * 2;
										pts[ 2 ].y = end.y + seg;
									break;

									case LINK_LEFT:
										pts[ 1 ].x = end.x - seg * 2;
										pts[ 1 ].y = end.y - seg;
										pts[ 2 ].x = end.x - seg * 2;
										pts[ 2 ].y = end.y + seg;
									break;

									case LINK_TOP:
										pts[ 1 ].x = end.x - seg;
										pts[ 1 ].y = end.y - seg * 2;
										pts[ 2 ].x = end.x + seg;
										pts[ 2 ].y = end.y - seg * 2;
									break;

									case LINK_BOTTOM:
										pts[ 1 ].x = end.x - seg;
										pts[ 1 ].y = end.y + seg * 2;
										pts[ 2 ].x = end.x + seg;
										pts[ 2 ].y = end.y + seg * 2;
									break;

									default:
										drawArrow = FALSE;
									break;
								}
							}
							break;
						}

						dc->MoveTo( start );
						dc->LineTo( end );

						pts[ 0 ].x = end.x;
						pts[ 0 ].y = end.y;

						if( drawArrow )
							dc->Polygon( pts, 3 );

						CString str = link->title;
						if( str.GetLength() )
						{
							dc->SelectObject( &font );
							int mode = dc->SetBkMode( TRANSPARENT );

							CRect rect( start, end );
							rect.NormalizeRect();
							int cy = round( 14.0 * zoom );
							int cut = round( ( double ) GetMarkerSize().cx * zoom / 2 );
							CRect r( rect.right - cut, rect.top, rect.right - ( rect.Width() + cut ), rect.bottom );
							if(rect.top==rect.bottom)
							{
								CRect r( rect.left, rect.top - ( cy + cut ), rect.right, rect.bottom );
								r.NormalizeRect();
								dc->DrawText( str, r, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_CENTER );
							}
							else
							{
								CRect r( rect.right - cut, rect.top, rect.right - ( cy * str.GetLength() + cut ), rect.bottom );
								r.NormalizeRect();
								dc->DrawText( str, r, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_RIGHT );
							}

							dc->SetBkMode( mode );
							dc->SelectStockObject( DEFAULT_GUI_FONT );

						}
					}
				}
			}
			main->DrawObject( dc, zoom );
		}
	}
}

void CScenarioEditor::SaveObjects( CStringArray& stra )
{

	CDiagramEditor::SaveObjects( stra );
#ifdef WEBAPP
	CItemEntityContainer* objs = (CItemEntityContainer * )GetDiagramEntityContainer();
#else
	CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
	if( objs )
	{
		int max = objs->GetLinks();
		for( int t = 0 ; t < max ; t++ )
		{
			CItemLink* link = objs->GetLinkAt( t );
			stra.Add( link->GetString() );
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditor message handlers

void CScenarioEditor::OnMouseMove( UINT nFlags, CPoint point )
{
	if( ( GetInteractMode() == MODE_MOVING ) || ( GetInteractMode() == MODE_RESIZING && GetSelectCount() == 1 ) )
		SetRedraw( FALSE );
	CDiagramEditor::OnMouseMove( nFlags, point );
#ifdef WEBAPP
	if( this->m_bClicked)
	{
		CTestWeb1Ctrl *pDoc = (CTestWeb1Ctrl *)GetDocument();
		if( pDoc )
		{
			if( pDoc->m_pTestFormView )
				pDoc->m_pTestFormView->m_endPoint = point;
		}
	}
#endif
	if( GetInteractMode() == MODE_RESIZING )
	{
		if( GetSelectCount() == 1 )
		{
#ifdef WEBAPP
			CItemLinkableLineSegment* obj = (CItemLinkableLineSegment*)GetSelectedObject();
#else
			CItemLinkableLineSegment* obj = dynamic_cast< CItemLinkableLineSegment* >( GetSelectedObject() );
#endif
			if( obj )
			{
				if( obj->GetLeft() != obj->GetRight() && obj->GetTop() != obj->GetBottom() )
				{
					if( fabs( obj->GetLeft() - obj->GetRight() ) > fabs( obj->GetTop() - obj->GetBottom() ) )
						obj->SetBottom( obj->GetTop() );
					else
						obj->SetRight( obj->GetLeft() );
				}
			}
		}
	}
	
	if( GetInteractMode() == MODE_MOVING || GetInteractMode() == MODE_RESIZING)
	{
		ModifyLinkedPositions();
		SetRedraw( TRUE );
		RedrawWindow();
	}
}

void CScenarioEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDiagramEditor::OnLButtonDown(nFlags, point);
#ifdef WEBAPP
	CTestWeb1Ctrl *pDoc = (CTestWeb1Ctrl *)GetDocument();
	if( pDoc )
	{
		if( pDoc->m_pTestFormView )
		{
			if( pDoc->m_pTestFormView->m_iSelectedItem != IDT_NULL)
				this->m_bClicked = TRUE;
		}
	}
#else
	if( ((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem != IDT_NULL )
		this->m_bClicked = TRUE;
#endif
}
void CScenarioEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDiagramEditor::OnLButtonUp(nFlags, point);
#ifdef WEBAPP
	CTestWeb1Ctrl *pDoc = (CTestWeb1Ctrl *)GetDocument();
	if( pDoc )
	{
		if( pDoc->m_pTestFormView )
		{
			pDoc->m_pTestFormView->m_iSelectedItem = IDT_NULL;
			this->m_bClicked = FALSE;
		}
	}
#else
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_NULL;
	this->m_bClicked = FALSE;
#endif
}
void CScenarioEditor::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDiagramEditor::OnRButtonUp(nFlags, point);
}
void CScenarioEditor::OnRButtonDown(UINT nFlags, CPoint point)
{
	CDiagramEditor::OnRButtonDown(nFlags, point);
#ifdef WEBAPP
	CTestWeb1Ctrl *pDoc = (CTestWeb1Ctrl *)GetDocument();
	if( pDoc )
	{
		if( pDoc->m_pTestFormView )
		{
			if( pDoc->m_pTestFormView->m_iSelectedItem != IDT_NULL )
			{
				RemoveLastObject();
			}
			pDoc->m_pTestFormView->m_iSelectedItem = IDT_NULL;
		}
	}
#else
	if( ((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem != IDT_NULL )
	{
		RemoveLastObject();
	}
	((CScenarioEditorApp*)AfxGetApp())->m_iSelectedItem = IDT_NULL;
#endif
}

void CScenarioEditor::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{

	SetRedraw( FALSE );
	CDiagramEditor::OnKeyDown( nChar, nRepCnt, nFlags );

	ModifyLinkedPositions();
	SetRedraw( TRUE );
	RedrawWindow();

}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditor private helpers

void CScenarioEditor::ModifyLinkedPositions()
{
	int t;
	int max = GetObjectCount();
	CScenarioEntity* obj;
	for( t = 0 ; t < max ; t++ )
	{
#ifdef WEBAPP
		obj = (CScenarioEntity*)GetObject( t );
#else
		obj = dynamic_cast< CScenarioEntity* >( GetObject( t ) );
#endif
		//if( obj && obj->IsSelected() )
		//	AdjustLinkedObjects( obj );
	}

	max = GetObjectCount();
	for( t = 0 ; t < max ; t++ )
	{
#ifdef WEBAPP
		obj = (CScenarioEntity*)GetObject( t );
#else
		obj = dynamic_cast< CScenarioEntity* >( GetObject( t ) );
#endif
		if( obj )
			obj->SetMoved( FALSE );
	}
}

void CScenarioEditor::AdjustLinkedObjects( CScenarioEntity* parent, CScenarioEntity* filter )
{
	parent->SetMoved( TRUE );
	BOOL moved = FALSE;

	CString name1 = parent->GetName();
	CString name2;

	double sizediff = 0.0;
#ifdef WEBAPP
	CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
	CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
	if( objs )
	{
		int max = objs->GetLinks();
		for( int t = 0 ; t < max ; t++ )
		{
			CItemLink* link = objs->GetLinkAt( t );
			if( link )
			{
				int fromtype = link->fromtype;
				int totype = link->totype;

				name2= _T( "" );
				if( link->from == parent->GetName() )
					name2 = link->to;
				else if( link->to == parent->GetName() )
				{
					fromtype = link->totype;
					totype = link->fromtype;
					name2 = link->from;
				}

				if( name2.GetLength() )
				{
					CScenarioEntity* obj = GetNamedObject( name2 );
					if( obj && obj != filter && !obj->GetMoved() )
					{
						CPoint source;
						CPoint target;

						source = parent->GetLinkPosition( fromtype );
						target = obj->GetLinkPosition( totype );

						moved = FALSE;
						switch( fromtype )
						{
							case LINK_LEFT:
								sizediff = source.y - target.y;
								obj->MoveRect( 0, source.y - target.y );
							break;
							case LINK_RIGHT:
								sizediff = source.y - target.y;
								obj->MoveRect( 0, source.y - target.y );
							break;
							case LINK_TOP:
								sizediff = source.x - target.x;
								obj->MoveRect( source.x - target.x, 0 );
							break;
							case LINK_BOTTOM:
								sizediff = source.x - target.x;
								obj->MoveRect( source.x - target.x, 0 );
							break;
							default:
							switch( totype )
							{
								case LINK_LEFT:
									sizediff = source.y - target.y;
									obj->MoveRect( 0, source.y - target.y );
								break;
								case LINK_RIGHT:
									sizediff = source.y - target.y;
									obj->MoveRect( 0, source.y - target.y );
								break;
								case LINK_TOP:
									sizediff = source.x - target.x;
									obj->MoveRect( source.x - target.x, 0 );
								break;
								case LINK_BOTTOM:
									sizediff = source.x - target.x;
									obj->MoveRect( source.x - target.x, 0 );
								break;
							}
							break;
						}

						if( sizediff )
							AdjustLinkedObjects( obj, parent );

					}
				}
			}
		}
	}
}

CScenarioEntity* CScenarioEditor::GetNamedObject( const CString& name ) const
{
	CDiagramEntity* result = NULL;

	int count = GetObjectCount();
	CDiagramEntity* obj;
	for( int t = 0 ; t < count ; t++ )
	{
		obj = GetObject( t );
		if( obj && obj->GetName() == name )
			result = obj;
	}
#ifdef WEBAPP
	return (CScenarioEntity*)result;
#else
	return dynamic_cast< CScenarioEntity* >( result );
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditor command handlers

void CScenarioEditor::OnNumber()
{
	int dtmfval;
	PMSG pMsg = (PMSG)this->GetCurrentMessage();
#ifdef WEBAPP
	CTestWeb1Ctrl *pDoc = (CTestWeb1Ctrl *)GetDocument();
	CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
	CScenarioEditorDoc * pDoc = (CScenarioEditorDoc *)GetDocument();
	CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
	if( !pDoc )
		return;
	
	CDiagramEntity* primary = objs->GetPrimaryLink();
	CDiagramEntity* secondary = objs->GetSecondaryLink();
	CScenarioItem * pItem = (CScenarioItem*)&(pDoc ->cItem);
	if( !primary || !secondary )
		return ;
	if( primary->GetItemType() != IDT_HUB )
		return;
	if( !pDoc || !pMsg)
		return;
	if( pMsg )
	{
		switch(pMsg->wParam)
		{
		case IDM_DTMF_1:
			dtmfval = 1;
			break;
		case IDM_DTMF_2:
			dtmfval = 2;
			break;
		case IDM_DTMF_3:
			dtmfval = 3;
			break;
		case IDM_DTMF_4:
			dtmfval = 4;
			break;
		case IDM_DTMF_5:
			dtmfval = 5;
			break;
		case IDM_DTMF_6:
			dtmfval = 6;
			break;
		case IDM_DTMF_7:
			dtmfval = 7;
			break;
		case IDM_DTMF_8:
			dtmfval = 8;
			break;
		case IDM_DTMF_9:
			dtmfval = 9;
			break;
		case IDM_DTMF_0:
			dtmfval = 0;
			break;
		case IDM_DTMF_STAR:
			dtmfval = 10;
			break;
		case IDM_DTMF_SHARP:
			dtmfval = 11;
			break;
		}
	}
	pItem->AddItemToGoAndDtmf(primary->GetCurID(), secondary->GetCurID(), dtmfval);
	CItemLink *pLink = objs->FindLink(primary, secondary);
	if( pLink )
	{
		pLink->title.Format("%d->%d[%d]", primary->GetCurID(), secondary->GetCurID(), dtmfval);
	}
}

void CScenarioEditor::OnLink() 
{
	if( CanLink() )
	{
		// Get the two selected objects 
#ifdef WEBAPP
		CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
		CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
		CScenarioEntity* primary = objs->GetPrimarySelected();
		CScenarioEntity* secondary = objs->GetSecondarySelected();

		if( primary && secondary )
		{
			GetDiagramEntityContainer()->Snapshot();
			char title[64];
#ifdef WEBAPP
			CTestWeb1Ctrl *pDoc = (CTestWeb1Ctrl *)GetDocument();
#else
			CScenarioEditorDoc *pDoc=(CScenarioEditorDoc *)this->GetDocument();
#endif
			if( !pDoc )
			{
				return;
			}
			memset(title, 0, 64);
			switch(primary->GetItemType())
			{
			case IDT_RETRIAL:
			case IDT_LOGICAL:
			//case IDT_MATH:
			case IDT_CONDITION:
			case IDT_PLUG:
			case IDT_CALLERR:
				{
					BOOL bFalseGoBoxShow=TRUE;
					CItem cItem;
					char sValue[32];
					if( pDoc->cItem.GetItem(primary->GetCurID(), cItem) )
					{
						memset(sValue, 0, 32);
						if( cItem.GetValue("COMMAND", sValue) )
						{
							if( !memcmp(sValue, "2", 1) )
							{
								bFalseGoBoxShow = FALSE;
							}
						}
					}
					if(primary->GetItemType() == IDT_RETRIAL
						&& !bFalseGoBoxShow)
					{
						
						pDoc->cItem.SetGo(primary->GetCurID(),secondary->GetCurID());
						primary->SetGo(secondary->GetCurID());
						
					}
					else
					{
						if(pDoc->cItem.GetFalseGo(primary->GetCurID()) < 0 )
						{
							if( pDoc->cItem.GetGo(primary->GetCurID()) < 0 )
							{
								if(AfxMessageBox("Is this false_goto?",MB_YESNO) == IDYES)
								{
									wsprintf(title,"%s", "FALSE");
									pDoc->cItem.SetFalseGo(primary->GetCurID(),secondary->GetCurID());
									primary->SetFalseGo(secondary->GetCurID());
									//objs->SetTitle("FALSE");
								}
								else
								{
									pDoc->cItem.SetGo(primary->GetCurID(),secondary->GetCurID());
									primary->SetGo(secondary->GetCurID());
									wsprintf(title,"%s", "TRUE");
								}
							}
							else
							{
								AfxMessageBox("This is case of false_goto!");
								primary->SetFalseGo(secondary->GetCurID());
								pDoc->cItem.SetFalseGo(primary->GetCurID(),secondary->GetCurID());
								wsprintf(title,"%s", "FALSE");
							}
						}
						else
						{
							primary->SetGo(secondary->GetCurID());
							pDoc->cItem.SetGo(primary->GetCurID(),secondary->GetCurID());
							wsprintf(title,"%s", "TRUE");
						}
					}
				}
				break;
			case IDT_HUB:
				{
					if(pDoc->cItem.GetFalseGo(primary->GetCurID()) < 0 )
					{	
						if(AfxMessageBox("Is this false_goto?",MB_YESNO) == IDYES)
						{
							wsprintf(title,"%s", "FALSE");
							pDoc->cItem.SetFalseGo(primary->GetCurID(),secondary->GetCurID());
							primary->SetFalseGo(secondary->GetCurID());
						}
						else
							pDoc->cItem.AddItemToGoAndDtmf(primary->GetCurID(), secondary->GetCurID());
					}
					else
					{
						pDoc->cItem.AddItemToGoAndDtmf(primary->GetCurID(), secondary->GetCurID());
					}
				}
				break;
			case IDT_TIMECHK:
			default:
				{
					pDoc->cItem.SetGo(primary->GetCurID(),secondary->GetCurID());
					primary->SetGo(secondary->GetCurID());
				}
				break;
			}
			if( objs->CreateLink( primary, secondary, _T(title) ) )
			{

				int max = GetObjectCount();
				for( int t = 0 ; t < max ; t++ )
				{
#ifdef WEBAPP
					CScenarioEntity* obj = (CScenarioEntity*)GetObject( t );
#else
					CScenarioEntity* obj = dynamic_cast< CScenarioEntity* >( GetObject( t ) );
#endif
					if( obj )
						obj->SetMoved( FALSE );
				}
				RedrawWindow();
			}
		}
	}
}

void CScenarioEditor::OnUnlink() 
{
	if( IsLinked() )
	{
		// Get the two selected objects 
#ifdef WEBAPP
		CTestWeb1Ctrl *pDoc = (CTestWeb1Ctrl *)GetDocument();
		CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
		CScenarioEditorDoc * pDoc = (CScenarioEditorDoc *)GetDocument();
		CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
		if( !pDoc )
		{
			return;
		}
		CDiagramEntity* primary = objs->GetPrimaryLink();;
		CDiagramEntity* secondary = objs->GetSecondaryLink();
		if( primary && secondary )
		{
			CItemLink* link = objs->FindLink( primary, secondary );
			if( link )
			{
				if( primary->GetItemType() != IDT_HUB )
				{
					if( primary->GetGo() == secondary->GetCurID() )
					{
						int LinkedCount = primary->GetLinkedCount();
						primary->SetLinkedCount(--LinkedCount);
						primary->SetGo();
						if( pDoc )
							pDoc->cItem.SetGo(primary->GetCurID(), -1);
					}
					else if( secondary->GetGo()== primary->GetCurID() )
					{
						int LinkedCount = secondary->GetLinkedCount();
						secondary->SetLinkedCount(--LinkedCount);
						secondary->SetGo();
						if( pDoc )
							pDoc->cItem.SetGo(secondary->GetCurID(), -1);
					}
					else if(primary->GetFalseGo() == secondary->GetCurID() )
					{
						int LinkedCount = primary->GetLinkedCount();
						primary->SetLinkedCount(--LinkedCount);
						primary->SetFalseGo();
						if( pDoc )
							pDoc->cItem.SetFalseGo(primary->GetCurID(), -1);
					}
					else if(secondary->GetFalseGo() == primary->GetCurID() )
					{
						int LinkedCount = secondary->GetLinkedCount();
						secondary->SetLinkedCount(--LinkedCount);
						secondary->SetFalseGo();
						if( pDoc )
							pDoc->cItem.SetFalseGo(secondary->GetCurID(), -1);
					}
				}
				else
				{
					int LinkedCount = primary->GetLinkedCount();
					if( pDoc->cItem.GetFalseGo(primary->GetCurID()) != secondary->GetCurID() )
						pDoc->cItem.DeleteToGoAndDtmf(primary->GetCurID(), secondary->GetCurID() );
					else
					{
						pDoc->cItem.SetFalseGo(primary->GetCurID(),-1);
						primary->SetFalseGo();
					}
					primary->SetLinkedCount(--LinkedCount);
				}
				GetDiagramEntityContainer()->Snapshot();
				objs->DeleteLink( link );
				RedrawWindow();
			}
		}
	}

}

void CScenarioEditor::OnLinkDirection() 
{
	if( IsLinked() )
	{
		// Get the two selected objects 
#ifdef WEBAPP
		CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
		CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
		CScenarioEntity* primary = objs->GetPrimaryLink();
		CScenarioEntity* secondary = objs->GetSecondaryLink();
		if( primary && secondary )
		{
			CItemLink* link = objs->FindLink( primary, secondary );

			if( link )
			{
				GetDiagramEntityContainer()->Snapshot();
				CString saved = link->from;
				link->from = link->to;
				link->to = saved;
				int fromtype = link->fromtype;
				int totype = link->totype;
				link->fromtype = totype;
				link->totype = fromtype;
				RedrawWindow();
			}
		}
	}
}

void CScenarioEditor::OnLinkProperties() 
{
	if( IsLinked() )
	{
		// Get the two selected objects 
#ifdef WEBAPP
		CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
		CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
		CScenarioEntity* primary = objs->GetPrimaryLink();
		CScenarioEntity* secondary = objs->GetSecondaryLink();
		if( primary && secondary )
		{
			CItemLink* link = objs->FindLink( primary, secondary );
			if( link )
			{
				//CItemLinkPropertiesDialog	dlg;
				//dlg.m_linkTitle = link->title;
				//if( dlg.DoModal() )
				//{
				//	GetDiagramEntityContainer()->Snapshot();
				//	link->title = dlg.m_linkTitle;
				//	RedrawWindow();
				//	SetFocus();
				//}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CScenarioEditor implementation

BOOL CScenarioEditor::CanLink()
{
	BOOL result = FALSE;
#ifdef WEBAPP
	CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
	CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
	if( objs )
		result = objs->CanLink();

	return result;

}

BOOL CScenarioEditor::IsLinked()
{
	BOOL result = FALSE;
#ifdef WEBAPP
	CItemEntityContainer* objs = (CItemEntityContainer *)GetDiagramEntityContainer();
#else
	CItemEntityContainer* objs = dynamic_cast< CItemEntityContainer * >( GetDiagramEntityContainer() );
#endif
	if( objs )
		result = objs->IsLinked();
	return result;

}

void  CScenarioEditor::SetDocument(PVOID pDoc)
{
	CDiagramEditor::SetDocument(pDoc);
}

