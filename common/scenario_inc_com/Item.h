// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__76AD37C9_6626_4544_B9D2_886C0C9809FB__INCLUDED_)
#define AFX_ITEM_H__76AD37C9_6626_4544_B9D2_886C0C9809FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <algorithm>
#include <string>

typedef struct _item_component
{
	_item_component();
	~_item_component();
	void			init();
	_item_component &operator=(const _item_component & i);
	int				operator==(const _item_component & i);
	int				operator>(const _item_component & i);
	int				operator<(const _item_component & i);
	int				operator==(const char * i);
	int				operator>(const char * i);
	int				operator<(const char * i);
	char			name[128];
	char			value[256];
}ITEMCOMPONENT,*PITEMCOMPONENT;

typedef struct _array_go_to
{
	_array_go_to();
	~_array_go_to();
	void			init();
	_array_go_to	&operator=(const _array_go_to &i);
	int				operator==(const _array_go_to & i);
	int				operator<(const _array_go_to & i);
	int				operator>(const _array_go_to & i);
	int				operator==(const int i);
	int				operator<(const int i);
	int				operator>(const int i);
	int				mapChar;
	int				gotoNum;
}ARRAYGOTO,*PARRAYGOTO;

class CItem  
{
public:
	CItem();
	virtual ~CItem();
	//operator
	CItem			&operator=(const CItem & i);
	int				operator==(const CItem & i);
	int				operator>(const CItem & i);
	int				operator<(const CItem & i);
	int				operator==(const int count);
	int				operator>(const int count);
	int				operator<(const int count);

	// function
	PITEMCOMPONENT	GetComponent(IN OUT PITEMCOMPONENT com); // sequencially get component
	void			DelItem(IN char *name=NULL);
	PITEMCOMPONENT	GetValue(IN char *name,OUT char *value,IN PITEMCOMPONENT pPre=NULL);
	BOOL			AddItem(IN char *szName,IN int iValue);
	BOOL			AddItem(IN char *szName,IN char *value);
	BOOL			SetItem(IN char *szName,IN char *value);
	BOOL			SetItem(IN char *szName,IN int iValue);
	void			SetType(IN int _type);
	void			SetCount(IN int _count);
	int				GetType();
	int				GetCount();
	void			init();
	void			free();
	int				GetGo();
	int				GetFalseGo();
	void			SetGo(IN int _go=-1);
	void			SetFalseGo(IN int _falsego=-1);
	int				GetMtype();
	void			SetMtype(IN int _mtype=-1);
	
	// function related with array_goto.
	int				AddArrayGoto(IN int iGoto,IN int idtmf=-1);
	void			DelArrayGoto(int iGoto);
	void			SetKeyMap(int iGoto, int iDTMF);
	int				GetArrayGotoSize();
	int				AllDeleteArrayGoto();
	int				GetGotoAndDTMFByCount(IN int iCount,OUT int &iGoto,OUT int &iDtmf);
private:
	int				count;
	int				type;
	int				go;
	int				falsego;
	int				mtype;
	std::vector<ITEMCOMPONENT> component;
	std::vector<ARRAYGOTO> array_goto;
};

#endif // !defined(AFX_ITEM_H__76AD37C9_6626_4544_B9D2_886C0C9809FB__INCLUDED_)
