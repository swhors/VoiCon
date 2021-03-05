// ListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "typedef.h"

#include "resource.h"

#ifndef WEBAPP
#include "scenarioeditor.h"
#endif

#include "ListBoxEx.h"

#include <vector>
#include <algorithm>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

std::vector<std::string> listString;

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx

CListBoxEx::CListBoxEx()
{
	this->iSelected = -1;
	bLButtonPressed = FALSE;
}

CListBoxEx::~CListBoxEx()
{
}


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	//{{AFX_MSG_MAP(CListBoxEx)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_COMMAND(IDM_SETCONF,OnRemoveItem)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONUP()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx message handlers

void CListBoxEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//MoveListString();
	bLButtonPressed = FALSE;
	//this->iSelected = -1;
	int iSize = listString.size();
	if( iSize > 0 )
	{
		std::vector<std::string>::iterator begin,end;
		begin = listString.begin();
		end = listString.end();
		listString.erase(begin,end);
	}
	CListBox::OnLButtonUp(nFlags, point);
}

void CListBoxEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonDown(nFlags, point);
	bLButtonPressed = TRUE;
	this->iSelected = GetCurSel();
	if( this->iSelected >= 0 )
	{
		memset(this->sSelected, 0, 256);
		GetText(this->iSelected, this->sSelected );
	}
}

void CListBoxEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	this->MoveListString();
	CListBox::OnMouseMove(nFlags, point);
}

void CListBoxEx::MoveListString()
{
	int total;
	total = GetCount();
	if( this->iSelected >= 0 && this->bLButtonPressed  && this->iSelected < total)
	{
		CString csCurSel;
		char tBuf[256];
		int i;
		
		int iCurSel = GetCurSel();
		if( this->iSelected != iCurSel && iCurSel != LB_ERR)
		{
			if( iCurSel < this->iSelected  && iCurSel >= 0)
			{
				for( i = 0; i < total; i++)
				{
					memset(tBuf, 0, 256);
					GetText(i, tBuf);
					if( i == (this->iSelected -1 ) )
					{
						listString.push_back(this->sSelected);
						listString.push_back(tBuf);
						i++;
					}
					else if( i != this->iSelected )
					{
						listString.push_back(tBuf);
					}
				}
				this->iSelected --;
				for( i = (total-1) ; i >= 0; i--)
				{
					this->DeleteString(i);
				}
			}
			else if( iCurSel > this->iSelected && iCurSel < total)
			{
				for( i = 0; i < GetCount(); i++)
				{
					memset(tBuf, 0, 256);
					GetText(i, tBuf);
					if( i == (this->iSelected + 1 ) )
					{
						listString.push_back(tBuf);
						listString.push_back(this->sSelected);
					}
					else if ( i!= this->iSelected )
					{
						listString.push_back(tBuf);
					}
				}
				this->iSelected++;
				for( i = (total-1) ; i >= 0; i--)
				{
					this->DeleteString(i);
				}
			}
			int iSize = listString.size();
			if( iSize > 0 )
			{
				char *ch;
				std::vector<std::string>::iterator begin,end,cur;
				begin = listString.begin();
				end = listString.end();
				for( cur = begin; cur< end ; cur ++)
				{
					ch = (char*)cur->c_str();
					if( ch )
					{
						this->AddString(ch);
					}
				}
				listString.erase(begin,end);
			}
		}
	}
}

void CListBoxEx::OnRButtonUp(UINT nFlags, CPoint point) 
{
	HMENU hMenu;
	bLButtonPressed = FALSE;
	if( this->iSelected >= 0 )
	{
		hMenu = LoadMenu(AfxGetApp()->m_hInstance, 
			MAKEINTRESOURCE(IDR_MENU_LISTBOX));
		if( hMenu != NULL )
		{
			CPoint cPoint;
			HMENU hmenuTrackPopup = GetSubMenu(hMenu, 0);
			WINDOWPLACEMENT wPlace,wPlace1;
			GetActiveWindow()->GetWindowPlacement(&wPlace);
			GetWindowPlacement(&wPlace1);
			cPoint.x = point.x+wPlace.rcNormalPosition.left+wPlace1.rcNormalPosition.left ;
			cPoint.y = point.y+wPlace.rcNormalPosition.top + wPlace1.rcNormalPosition.top ;
			TrackPopupMenu(hmenuTrackPopup,
				TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				cPoint.x, cPoint.y, 0, m_hWnd, NULL);
			DestroyMenu(hMenu); 
		}
	}
	CListBox::OnRButtonUp(nFlags, point);
}

//LRESLUT CListBoxEx::OnRemoveItem(WPARAM wParam,LPARAM lParam)
void CListBoxEx::OnRemoveItem()
{
	this->DeleteString(iSelected);
}

