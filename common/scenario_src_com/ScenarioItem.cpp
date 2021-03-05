// ScenarioItem.cpp: implementation of the CScenarioItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "typedef.h"
#ifndef WEBAPP
#include "scenarioeditor.h"
#endif
#include "ScenarioItem.h"

#include "item.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScenarioItem::CScenarioItem()
{
	
}

CScenarioItem::~CScenarioItem()
{
	
}

void CScenarioItem::AddItem(IN CItem &item)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, item.GetCount());
	if( f == end )
	{
		scenario_item.push_back(item);
		begin = scenario_item.begin();
		end = scenario_item.end();
		std::sort(begin,end);
		f = std::find(begin,end,item.GetCount());
		if( f != end )
		{
			*f = item;
		}
	}
	else
	{
		*f = item;
	}
}

void CScenarioItem::AddItem(IN int iID,IN int iType)
{
	CItem item;
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f == end )
	{
		item.SetCount(iID);
		item.SetType(iType);
		if( iType == IDT_INIT ||
			iType == IDT_GOTO )
		{
			item.SetMtype(IDMT_GOTO);
		}
		scenario_item.push_back(item);
		begin = scenario_item.begin();
		end = scenario_item.end();
		std::sort(begin,end);
	}
}

void CScenarioItem::DelLastItem()
{
	std::vector<CItem>::iterator begin,end;
	begin = scenario_item.begin();
	end = scenario_item.end();
	if( begin != end )
	{
		scenario_item.erase(--end);
	}
}

void CScenarioItem::DelItem(IN int iID)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	if( iID < 0 )
	{
		for(f = begin; f < end ; f++)
		{
			f->free();
		}
		scenario_item.erase(begin,end);
	}
	else
	{
		f = std::find(begin, end, iID);
		if( f != end )
		{
			f->free();
			scenario_item.erase(f);
		}
	}
}

void CScenarioItem::SetConnectionInfo(IN int iID,IN int iVal)
{
	std::vector<CItem>::iterator begin,end,cur;
	begin = scenario_item.begin();
	end = scenario_item.end();
	for(cur = begin; cur < end ; cur ++ )
	{
		if( cur->GetFalseGo()== iID )
		{
			cur->SetFalseGo(iVal);
		}
		if( cur->GetGo() == iID )
		{
			cur->SetGo(iVal);
		}
	}
}

int CScenarioItem::GetItem(IN int iID,OUT CItem &item)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f != end )
	{
		item = *f;
		return 1;
	}
	return 0;
}

int CScenarioItem::GetGo(IN int iID)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f != end )
	{
		return f->GetGo();
	}
	return -1;
}

int CScenarioItem::GetFalseGo(IN int iID)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f != end )
	{
		return f->GetFalseGo();
	}
	return -1;
}

int CScenarioItem::SetFalseGo(IN int iID,IN int iFalseGo)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f != end )
	{
		f->SetFalseGo(iFalseGo);
		return 1;
	}
	return 0;
}

int CScenarioItem::SetGo(IN int iID,IN int iGo)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f != end )
	{
		f->SetGo(iGo);
		return 1;
	}
	return 0;
}

int CScenarioItem::InsertItemComponent(IN int iID,IN char *name,IN char *value)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f == end )
	{
		f->AddItem(name, value);
		return 1;
	}
	return 0;
}

int	CScenarioItem::InsertItemComponent(IN int iID,IN char *name,IN int iValue)
{
	char tbuf[128];
	wsprintf(tbuf,"%d", iValue);
	return this->InsertItemComponent(iID, name, tbuf);
}

void CScenarioItem::init()
{
}

CItem *CScenarioItem::FindItem(IN int iID,OUT CItem *item)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f != end )
	{
		if( item )
		{
			*item = *f;
		}
		//return (CItem*)f._Myptr;
		return (CItem*)f;
	}
	return NULL;
}

void CScenarioItem::free()
{
	std::vector<CItem>::iterator begin,end,cur;
	begin = scenario_item.begin();
	end = scenario_item.end();
	for(cur = begin; cur < end ; cur ++)
	{
		cur->free();
	}
	scenario_item.erase(begin,end);
}

int CScenarioItem::ReplaceItem(IN CItem &item)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, item.GetCount() );
	if( f != end )
	{
		*f = item;
		return 0;
	}
	return 0;
}

int CScenarioItem::WriteToFile(CString _csFileName)
{
	CFile cFile;
	CString csFileName;
	csFileName.Format("%s.scm", _csFileName);
	CString csBuf;
	if( cFile.Open(csFileName, CFile::modeWrite|CFile::modeCreate) )
	{
		std::vector<CItem>::iterator begin,end,cur;
		begin = scenario_item.begin();
		end = scenario_item.end();
		for(cur = begin;cur< end; cur ++)
		{
			PITEMCOMPONENT pCom=NULL;
			csBuf = "// -- Item Start \r\n";
			cFile.Write(csBuf,csBuf.GetLength());
			csBuf.Format("SELF:%d \r\n", cur->GetCount());
			cFile.Write(csBuf,csBuf.GetLength());
			csBuf.Format("TYPE:%d \r\n", cur->GetType());
			cFile.Write(csBuf,csBuf.GetLength());
			if( cur->GetMtype() >= 0 )
			{
				csBuf.Format("MTYPE:%d \r\n", cur->GetMtype());
				cFile.Write(csBuf,csBuf.GetLength());
			}
			csBuf.Format("TARGET0:%d \r\n", cur->GetGo());
			cFile.Write(csBuf,csBuf.GetLength());
			if( cur->GetFalseGo() >= 0 )
			{
				csBuf.Format("FALSEGO:%d \r\n", cur->GetFalseGo());
				cFile.Write(csBuf,csBuf.GetLength());
			}
			pCom = cur->GetComponent(pCom);
			while(pCom )
			{
				csBuf.Format("%s:%s\r\n", pCom->name, pCom->value);
				pCom = cur->GetComponent(pCom);
				cFile.Write(csBuf,csBuf.GetLength());
			}
			if( cur->GetType() == IDT_HUB )
			{
				int iSize = cur->GetArrayGotoSize();
				int iToGo,iDtmf;
				for(int i = 0; i < iSize ; i++ )
				{
					if( cur->GetGotoAndDTMFByCount(i, iToGo, iDtmf ))
					{
						csBuf.Format("MPOINT:%d\r\n", iToGo);
						cFile.Write(csBuf, csBuf.GetLength() );
						csBuf.Format("MDTMF:%d\r\n", iDtmf);
						cFile.Write(csBuf, csBuf.GetLength() );
					}
				}
			}
			csBuf = "// -- Item End \r\n";
			cFile.Write(csBuf,csBuf.GetLength());
		}
		cFile.Close();
	}
	return 0;
}

int CScenarioItem::ReadFromFile(CString _csFileName)
{
	int lastItem=0,iPreMPoint=-1;
	int	retVal;
	CStdioFile cFile;
	CString csFileName;
	char *tbuf;
	csFileName.Format("%s.scm", _csFileName);
	CString csBuf;
	CItem item;
	retVal = 0;
	if( cFile.Open(csFileName, CFile::modeRead) )
	{
		while(cFile.ReadString(csBuf) )
		{
			if( !csBuf.Compare("// -- Item Start ") )
			{
				item.init();
			}
			else if( !csBuf.Find("SELF:") )
			{
				tbuf = csBuf.GetBuffer( 256);
				//wsprintf(value, "%s", tbuf[5]);
				item.SetCount(atoi(tbuf+5));
			}
			else if( !csBuf.Find("TYPE:") )
			{
				tbuf = csBuf.GetBuffer( 256 );
				//wsprintf(value, "%s", tbuf[5]);
				item.SetType(atoi(tbuf+5));
			}
			else if( !csBuf.Find("MTYPE:") )
			{
				tbuf = csBuf.GetBuffer( 256 );
				item.SetMtype(atoi(tbuf+6));
			}
			else if( !csBuf.Find("FALSEGO:") )
			{
				tbuf = csBuf.GetBuffer( 256);
				item.SetFalseGo(atoi(tbuf+8));
			}
			else if( !csBuf.Find("TARGET0:") )
			{
				tbuf = csBuf.GetBuffer( 256);
				item.SetGo(atoi(tbuf+8));
			}
			else if( !csBuf.Find("MPOINT:"))
			{
				if( item.GetType() == IDT_HUB )
				{
					tbuf = csBuf.GetBuffer( 256);
					iPreMPoint=atoi(tbuf+7);
					item.AddArrayGoto(iPreMPoint);
				}
			}
			else if( !csBuf.Find("MDTMF:"))
			{
				if( item.GetType() == IDT_HUB && iPreMPoint >= 0)
				{
					tbuf = csBuf.GetBuffer( 256);
					int key = atoi(tbuf+6);
					if( !item.AddArrayGoto(iPreMPoint, key) )
					{
						item.SetKeyMap(iPreMPoint, key );
					}
				}
			}
			else if( !csBuf.Compare("// -- Item End ") )
			{
				this->AddItem(item);
				if( lastItem < item.GetCount() )
					lastItem = item.GetCount();
			}
			else
			{
				char value[256];
				char name[128];
				memset(value, 0, 256);
				memset(name, 0, 128);
				int iLocation = csBuf.Find(":");
				tbuf = csBuf.GetBuffer( 256);
				memcpy(name, tbuf, (iLocation ));
				wsprintf(value, "%s",(tbuf+iLocation+1));
				item.AddItem(name, value);
			}
		}
		cFile.Close();
		return lastItem;
	}
	return -1;
}

void CScenarioItem::DeleteItemConnectedLine(IN int iID,IN int delID)
{
	std::vector<CItem>::iterator begin,end,f;
	begin = scenario_item.begin();
	end = scenario_item.end();
	f = std::find(begin, end, iID);
	if( f != end )
	{
		if( f->GetFalseGo() == delID )
			f->SetFalseGo();
		if( f->GetGo() == delID )
			f->SetGo();
	}
}

int CScenarioItem::HaveTop()
{
	std::vector<CItem>::iterator begin, end, cur;
	begin = scenario_item.begin();
	end = scenario_item.end();
	for(cur =begin; cur < end ; cur ++ )
	{
		if( cur->GetCount() == 0 )
			return 1;
	}
	return 0;
}

int CScenarioItem::AddItemToGoAndDtmf(IN int iID,IN int iToGo,IN int iDtmf)
{
	CItem *pItem;
	pItem = this->FindItem(iID, NULL);
	if( pItem )
	{
		if( pItem->AddArrayGoto(iToGo, iDtmf) )
		{
			return 1;
		}
		else
		{
			pItem->SetKeyMap(iToGo, iDtmf);
			return 1;
		}
	}
	return 0;
}
void CScenarioItem::DeleteToGoAndDtmf(IN int iID,IN int iToGo)
{
	CItem *pItem;
	pItem = this->FindItem(iID, NULL);
	if( pItem )
	{
		pItem->DelArrayGoto(iToGo);
	}
}
int CScenarioItem::GetToGoAndDtmfByCount(IN int iID,OUT int &iToGo,OUT int &iDtmf,IN int icount)
{
	CItem *pItem;
	pItem = this->FindItem(iID, NULL );
	if( pItem )
	{
		return pItem->GetGotoAndDTMFByCount(icount, iToGo, iDtmf);
	}
	return 0;
}

int CScenarioItem::GetSizeToGoAndDtmf(IN int iID,IN int iToGo)
{
	CItem *pItem;
	pItem = this->FindItem(iID, NULL );
	if( pItem )
	{
		return pItem->GetArrayGotoSize();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// end of file...
//