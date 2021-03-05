// ScenarioItem.h: interface for the CScenarioItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENARIOITEM_H__722DA3D3_85E4_42C7_BF3E_E649527DC8F7__INCLUDED_)
#define AFX_SCENARIOITEM_H__722DA3D3_85E4_42C7_BF3E_E649527DC8F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <algorithm>

class CItem;

class CScenarioItem  
{
public:
	int HaveTop();
	CScenarioItem();
	virtual ~CScenarioItem();
	void	init();
	void	AddItem(IN int iID,IN int iType);
	void	AddItem(IN CItem &item);
	void	DelItem(IN int iID=-1);
	void	DelLastItem();
	int		GetItem(IN int iID,OUT CItem & item);
	int		InsertItemComponent(IN int iID,IN char *name,IN char *value);
	int		InsertItemComponent(IN int iID,IN char *name,IN int iValue);
	int		SetItemComponent(IN int iID,IN char *name,IN char *value);
	int		SetItemComponent(IN int iID,IN char *name,IN int iValue);
	CItem	*FindItem(IN int iID,OUT CItem *item=NULL);
	int		ReplaceItem(IN CItem & item);
	int		ReadFromFile(CString csFileName);
	int		WriteToFile(CString csFileName);
	void	DeleteItemConnectedLine(IN int ID,IN int delID);

	void	SetConnectionInfo(IN int iID,IN int iVal=-1);

	int		SetGo(IN int iID,IN int iGo);
	int		SetFalseGo(IN int iID,IN int iFalseGo);
	int		GetGo(IN int iID);
	int		GetFalseGo(IN int iID);
	
	void	free();
	// ToGo and Dtmf
	int		AddItemToGoAndDtmf(IN int iID,IN int iToGo,IN int iDtmf=-1);
	void	DeleteToGoAndDtmf(IN int iID,IN int iToGo);
	int		GetToGoAndDtmfByCount(IN int iID,OUT int &iToGo,OUT int &iDtmf,IN int icount=0);
	int		GetSizeToGoAndDtmf(IN int iID,IN int iToGo);
	std::vector<CItem> scenario_item;
};

#endif // !defined(AFX_SCENARIOITEM_H__722DA3D3_85E4_42C7_BF3E_E649527DC8F7__INCLUDED_)
