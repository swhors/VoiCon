// Item.cpp: implementation of the CItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifndef WEBAPP
#include "scenarioeditor.h"
#endif
#include "Item.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// _item_component Construction/Destruction
//////////////////////////////////////////////////////////////////////

_item_component::_item_component()
{
	memset(this->name, 0, 128);
	memset(this->value, 0, 256);
}

_item_component::~_item_component()
{
	memset(this->name, 0, 128);
	memset(this->value, 0, 256);	
}

_item_component & _item_component::operator=(const _item_component &i)
{
	wsprintf(this->name, "%s", i.name);
	wsprintf(this->value, "%s", i.value);
   return *this;
}

int _item_component::operator <(const _item_component &i)
{
	if( memcmp(this->name, i.name, strlen(i.name) ) < 0 )
	{
		if( memcmp(this->value, i.value, strlen( i.value) ) < 0 )
			return 1;
	}
	return 0;
}

int _item_component::operator >(const _item_component &i)
{
	if( memcmp(this->name, i.name, strlen(i.name) ) > 0 )
	{
		if( memcmp(this->value, i.value, strlen(i.value) ) > 0)
		{
			return 1;
		}
	}
	return 0;
}

int _item_component::operator ==(const _item_component &i)
{
	if( !memcmp(this->name, i.name, strlen(i.name) ) )
	{
		if( !memcmp(this->value, i.value, strlen(i.value) ) )
			return 1;
	}
	return 0;
}

int _item_component::operator==(const char * i)
{
	if( memcmp(this->name, i, strlen(i) ) == 0 )
		return 1;
	return 0;
}
int	_item_component::operator>(const char * i)
{
	if( memcmp(this->name, i, strlen(i) ) > 0 )
		return 1;
	return 0;
}
int _item_component::operator<(const char * i)
{
	if( memcmp(this->name, i, strlen(i) ) < 0 )
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////
// _array_go_to Construction/Destruction/Operation
//////////////////////////////////////////////////////////////////////
_array_go_to::_array_go_to()
{
	init();
}
_array_go_to::~_array_go_to()
{
	init();
}

void _array_go_to::init()
{
	this->gotoNum = -1;
	this->mapChar = -1;
}

_array_go_to & _array_go_to::operator =(const _array_go_to &i)
{
	this->gotoNum = i.gotoNum;
	this->mapChar = i.mapChar;
	return *this;
}

int _array_go_to::operator <(const int i)
{
	if( this->gotoNum < i )
		return 1;
	return 0;
}

int _array_go_to::operator ==(const int i)
{
	if( this->gotoNum == i)
		return 1;
	return 0;
}

int _array_go_to::operator >(const int i)
{
	if( this->gotoNum > i )
		return 1;
	return 0;
}

int _array_go_to::operator <(const _array_go_to & i)
{
	if( this->gotoNum < i.gotoNum)
		return 1;
	return 0;
}

int _array_go_to::operator ==(const _array_go_to & i)
{
	if( this->gotoNum == i.gotoNum)
		return 1;
	return 0;
}

int _array_go_to::operator >(const _array_go_to & i)
{
	if( this->gotoNum > i.gotoNum)
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CItem Construction/Destruction/Operation
//////////////////////////////////////////////////////////////////////

CItem::CItem()
{
	init();
}

CItem::~CItem()
{
	free();
}

CItem & CItem::operator =(const CItem & i)
{
	CItem * pItem = (CItem *)&i;
	std::vector<_item_component>::iterator begin, end, cur;
	std::vector<ARRAYGOTO>::iterator begin1, end1, cur1;
	this->count = i.count;
	this->type = i.type;
	this->init();
	begin = pItem->component.begin();
	end = pItem->component.end();
	for( cur=begin; cur < end;cur++ )
	{
		this->component.push_back(*cur);
	}
	begin1 = pItem->array_goto.begin();
	end1 = pItem->array_goto.end();
	for( cur1 = begin1; cur1 < end1 ; cur1++)
	{
		this->array_goto.push_back(*cur1);
	}
	this->falsego = i.falsego;
	this->go = i.go;
	this->mtype = i.mtype;
	return *this;
}

int CItem::operator < (const CItem & i)
{
	if( count < i.count)
		return 1;
	return 0;
}

int CItem::operator > (const CItem &i)
{
	if( this->count > i.count)
		return 1;
	return 0;
}

int CItem::operator == (const CItem &i)
{
	if( this->count == i.count)
		return 1;
	return 0;
}

int CItem::operator > (const int count)
{
	if( this->count == count )
		return 1;
	return 0;
}

int CItem::operator < (const int count)
{
	if( this->count < count )
		return 1;
	return 0;
}

int CItem::operator == (const int count)
{
	if( this->count == count )
		return 1;
	return 0;
}

BOOL CItem::SetItem(IN char *szName,IN char *value)
{
	std::vector<_item_component>::iterator begin, end,f;
	begin = this->component.begin();
	end = this->component.end();

	f = std::find(begin,end, szName);
	if( f !=end )
	{
		wsprintf(f->value, "%s", value);
		return TRUE;
	}
	return FALSE;
}

BOOL CItem::SetItem(IN char *szName,IN int iValue)
{
	char tbuf[128];
	wsprintf(tbuf,"%d", iValue);
	return SetItem(szName, tbuf);
}

BOOL CItem::AddItem(IN char *szName,IN char *value)
{
	std::vector<_item_component>::iterator begin, end;
	ITEMCOMPONENT item;
	wsprintf(item.name, "%s", szName);
	wsprintf(item.value, "%s", value);
	this->component.push_back(item);
	begin = this->component.begin();
	end = this->component.end();
	std::sort(begin, end);
	return TRUE;
}

BOOL CItem::AddItem(IN char *szName,IN int iValue)
{
	char tbuf[128];
	wsprintf(tbuf,"%d", iValue);
	return AddItem(szName, tbuf);
}

PITEMCOMPONENT CItem::GetComponent(IN OUT PITEMCOMPONENT com)
{
	std::vector<_item_component>::iterator begin, end;
	begin = this->component.begin();
	end = this->component.end();
	if( begin != end )
	{
		if( !com )
		{
			//return (PITEMCOMPONENT)begin._Myptr;
			return (PITEMCOMPONENT)begin;
		}
		else
		{
			//if( (com+1) != end._Myptr )
			if( (com+1) != end )
				//return (PITEMCOMPONENT)(com +1);
				return (PITEMCOMPONENT)(com +1);
		}
	}
	return NULL;
}

void CItem::DelItem(IN char *name)
{
	std::vector<_item_component>::iterator begin, end,f;
	begin = this->component.begin();
	end = this->component.end();
	if( name )
	{
		f = std::find(begin, end, name );
		if( f != end )
		{
			this->component.erase(f);
			begin = this->component.begin();
			end = this->component.end();
			std::sort(begin, end);
		}
	}
	else
	{
		this->component.erase(begin, end);
	}
}

PITEMCOMPONENT CItem::GetValue(IN char *name,OUT char *value,IN PITEMCOMPONENT pPre)
{
	std::vector<_item_component>::iterator begin, end,f;
	begin = this->component.begin();
	end = this->component.end();
	if( !pPre )
	{
		if( begin != end )
		{
			f = std::find(begin, end, name );
			if( f != end )
			{
				wsprintf(value, "%s", f->value);
				//ASSERT(sprintf(value, "%s", f->value));
				//return (PITEMCOMPONENT)f._Myptr;
				return (PITEMCOMPONENT)f;
			}
		}
	}
	else
	{
		if( begin != end )
		{
			f = std::find(begin, end, *pPre );
			if( f != end )
			{
				f++;
				if( f != end )
				{
					f = std::find(f, end, name);
					if( f != end )
					{
						wsprintf(value, "%s", f->value);
						//return (PITEMCOMPONENT)f._Myptr;
						return (PITEMCOMPONENT)f;
					}
				}
			}
		}
	}
	return NULL;
}

void CItem::SetCount(IN int _count)
{
	this->count = _count;
}

void CItem::SetType(IN int _type)
{
	this->type = _type;
}

int CItem::GetType()
{
	return type;
}

int CItem::GetCount()
{
	return count;
}

void CItem::init()
{
	std::vector<_item_component>::iterator begin, end;
	begin = this->component.begin();
	end = this->component.end();
	if( begin != end )
	{
		this->component.erase(begin, end);
	}
	this->falsego = -1;
	this->go = -1;
	this->mtype = -1;
	std::vector<ARRAYGOTO>::iterator begin1, end1;
	begin1 = this->array_goto.begin();
	end1 = this->array_goto.end();
	if( begin1 != end1)
	{
		this->array_goto.erase(begin1, end1);
	}
}

void CItem::free()
{
	init();
}

int CItem::GetGo()
{
	return this->go;
}

int CItem::GetFalseGo()
{
	return this->falsego;
}

void CItem::SetGo(IN int _go)
{
	this->go = _go;
}

void CItem::SetFalseGo(IN int _falsego)
{
	this->falsego = _falsego;
}

void CItem::SetMtype(IN int _mtype)
{
	this->mtype = _mtype;
}

int CItem::GetMtype()
{
	return this->mtype;
}

int CItem::AddArrayGoto(IN int iGoto,IN int idtmf)
{
	std::vector<ARRAYGOTO>::iterator begin,end,f;
	begin = this->array_goto.begin();
	end = this->array_goto.end();
	f = std::find(begin, end, iGoto );
	if( iGoto < 0 )
		return 0;
	if( f == end )
	{
		ARRAYGOTO go;
		go.init();
		go.gotoNum = iGoto;
		go.mapChar = idtmf;
		this->array_goto.push_back(go);
		return 1;	
	}
	return 0;
}

void CItem::DelArrayGoto(int iGoto)
{
	std::vector<ARRAYGOTO>::iterator begin,end,f;
	begin = this->array_goto.begin();
	end = this->array_goto.end();
	f = std::find(begin, end, iGoto );
	
	if( f != end )
	{
		this->array_goto.erase(f);
	}
}

void CItem::SetKeyMap(int iGoto, int iDTMF)
{
	std::vector<ARRAYGOTO>::iterator begin,end,f;
	begin = this->array_goto.begin();
	end = this->array_goto.end();
	f = std::find(begin, end, iGoto );
	
	if( f != end )
	{
		f->mapChar=iDTMF;
	}
}

int CItem::GetArrayGotoSize()
{
	return this->array_goto.size();
}
int CItem::AllDeleteArrayGoto()
{
	std::vector<ARRAYGOTO>::iterator begin,end;
	begin = this->array_goto.begin();
	end = this->array_goto.end();
	if(begin != end )
	{
		this->array_goto.erase(begin, end );
		return 1;
	}
	return 0;
}

// 
int CItem::GetGotoAndDTMFByCount(IN int iCount,OUT int &iGoto,OUT int &iDtmf)
{
	std::vector<ARRAYGOTO>::iterator begin,end;
	begin = this->array_goto.begin();
	end = this->array_goto.end();
	if( begin != end )
	{
		if( (begin+iCount) != end )
		{
			iGoto = (begin+iCount)->gotoNum;
			iDtmf = (begin+iCount)->mapChar;
			return 1;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// end of file.
