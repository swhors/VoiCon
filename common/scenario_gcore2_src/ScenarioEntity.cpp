// ScenarioEntity.cpp: implementation of the CScenarioEntity class.
//
//////////////////////////////////////////////////////////////////////

/* ==========================================================================
	CScenarioEntity

	Author :		Johan Rosengren, Abstrakt Mekanik AB

	Date :			2004-04-29

	Purpose :		CScenarioEntity is a base class for linked objects used 
					by a CDiagramEditor.	

	Description :	The class contains a few virtual functions that derived 
					classes can implement:
					AllowLink and GetLinkPosition. AllowLink returns what 
					link types are allowed, one or more of the following 
					ORed together:
					LINK_TOP		Links are allowed to the 
									top of the object.
					LINK_BOTTOM		Links are allowed to the 
									bottom.
					LINK_LEFT		Links are allowed to the 
									left.
					LINK_RIGHT		Links are allowed to the 
									right.
					LINK_START		Links are allowed to the 
									start of a line (normally 
									the top-left corner of 
									the non-normalized bounding 
									rect).
					LINK_END		Links are allowed to the 
									end of a line (normally the
									bottom-right corner of the
									non-normalized bounding 
									rect).

					GetLinkPosition will return the position of a link type. 
					The coordinates are not zoomed, so if they should be 
					used for drawing, they must be multiplied with the 
					current zoom factor of the editor.

	Usage :			Use this class as a base class for linked objects.

   ========================================================================*/

#include "stdafx.h"
#ifdef WEBAPP
#include "resource.h"
#else
#include "ScenarioEditor.h"
#endif
#include "ScenarioEntity.h"

#include "LinkFactory.h"
#include "Tokenizer.h"
#include "ItemLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CScenarioEntity::CScenarioEntity()
{
	SetConstraints( CSize( 32, 32 ), CSize( -1, -1 ) );
	SetType( _T( "Scenario_Item" ) );
	SetTitle( _T( "title" ) );
	SetName( CLinkFactory::GetID() );
	
	SetMarkerSize( CSize( 6, 6 ) );
	// Setting fixed size
	SetName( CLinkFactory::GetID() );
	SetMoved( FALSE );
}

CScenarioEntity::~CScenarioEntity()
{
}

CDiagramEntity* CScenarioEntity::Clone()
{
	CScenarioEntity* obj = new CScenarioEntity;
	obj->Copy( this );
	return obj;
}

CDiagramEntity* CScenarioEntity::CreateFromString( const CString& str )
{
	CScenarioEntity* obj = new CScenarioEntity;
	if(!obj->FromString( str ) )
	{
		delete obj;
		obj = NULL;
	}

	return obj;
}

CString CScenarioEntity::GetString() const
{
	CString str;

	str.Format( _T( ",%i;" ), 
			GetSymbol()
		);

	str = GetDefaultGetString() + str;
	return str;
}

BOOL CScenarioEntity::FromString( const CString& str )
{
	BOOL result = FALSE;
	CString data( str );

	if( LoadFromString( data ) )
	{
		CTokenizer tok( data );

		int resid;
		tok.GetAt( 0, resid );

		SetSymbol( resid );
		result = TRUE;
	}
	return result;
}

int CScenarioEntity::GetHitCode( CPoint point ) const
{
	int result = DEHT_NONE;
	CRect rect = GetRect();
	if( rect.PtInRect( point ) )
		result = DEHT_BODY;
	return result;
}

void CScenarioEntity::DrawSelectionMarkers( CDC* dc, CRect rect ) const
{
	CDiagramEntity::DrawSelectionMarkers( dc, rect );
}

HCURSOR CScenarioEntity::GetCursor( int /*hit*/ ) const
{
	HCURSOR cursor = LoadCursor( NULL, IDC_SIZEALL );
	return cursor;
}

int CScenarioEntity::AllowLink()
{
	return LINK_ALL;
}

void CScenarioEntity::SetMoved( BOOL moved )
{
	m_moved = moved;
}

BOOL CScenarioEntity::GetMoved()
{
	return m_moved;
}

CPoint CScenarioEntity::GetLinkPosition( int type )
{
	CPoint point( -1, -1 );
	CRect rect = GetRect();
	switch( type )
	{
		case LINK_LEFT:
			point.x = rect.left;
			point.y = rect.top + round( ( double ) rect.Height() / 2 );
			break;
		case LINK_RIGHT:
			point.x = rect.right;
			point.y = rect.top + round( ( double ) rect.Height() / 2 );
			break;
		case LINK_TOP:
			point.x = rect.left + round( ( double ) rect.Width() / 2 );
			point.y = rect.top;
			break;
		case LINK_BOTTOM:
			point.x = rect.left + round( ( double ) rect.Width() / 2 );
			point.y = rect.bottom;
			break;
	}

	return point;
}

void CScenarioEntity::Copy( CDiagramEntity* obj )
{
	CDiagramEntity::Copy( obj );
#ifdef WEBAPP
	m_symbol = ((CScenarioEntity*)obj)->m_symbol;
#else
	m_symbol = static_cast< CScenarioEntity* >( obj )->m_symbol;
#endif
}

UINT CScenarioEntity::GetSymbol() const
{
	return m_symbol;
}

void CScenarioEntity::SetSymbol( UINT resid )
{
	m_symbol = resid;
	SetItemTypeByeResID(resid);
}

void CScenarioEntity::Draw( CDC* dc, CRect rect )
{
	ASSERT( m_symbol );

	dc->SelectStockObject( BLACK_PEN );
	dc->SelectStockObject( WHITE_BRUSH );

	CDC memDC;
	memDC.CreateCompatibleDC( dc );
	HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(m_symbol));
	if( hIcon )
	{
		CPoint point;
		point.x = rect.left;
		point.y = rect.top;
		dc->DrawState( point, CSize(32, 32),hIcon, DST_ICON|DSS_NORMAL, (CBrush *)NULL);
	}
}

void CScenarioEntity::SetItemTypeByeResID(int ResID)
{
	switch(ResID)
	{
	case IDI_ANDOR:
		SetItemType(IDT_LOGICAL);
		break;
	case IDI_VOICE_REC:
		SetItemType(IDT_VOICE_REC);
		break;
//	case IDI_ARROW:
//		SetItemType(IDT_ARROW);
//		break;
	case IDI_CIRCUIT:
		SetItemType(IDT_LOOP);
		break;
	case IDI_DTMF:
		SetItemType(IDT_INPUT);
		break;
	case IDI_EQUAL:
		SetItemType(IDT_CONDITION);
		break;
	case IDI_INIT:
		SetItemType(IDT_INIT);
		break;
	case IDI_IS:
		SetItemType(IDT_IS);
		break;
	case IDI_MATH:
		SetItemType(IDT_MATH);
		break;
	case IDI_PHONE:
		SetItemType(IDT_MAKE_CALL);
		break;
	case IDI_PLUG:
		SetItemType(IDT_PLUG);
		break;
	case IDI_RETRIAL:
		SetItemType(IDT_RETRIAL);
		break;
	case IDI_SPEAK:
		SetItemType(IDT_PLAY);
		break;
	case IDI_WATCH:
		SetItemType(IDT_TIMECHK);
		break;
	case IDI_GOTO:
		SetItemType(IDT_GOTO);
		break;
	case IDI_HUB:
		SetItemType(IDT_HUB);
		break;
	case IDI_PAGE:
		SetItemType(IDT_PAGE);
		break;
	case IDI_CALLERR:
		SetItemType(IDT_CALLERR);
		break;
	default:
		SetItemType(IDT_NULL);
		break;
	}
}
