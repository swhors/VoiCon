#include "stdafx.h"
#include "SStack.h"

/////////////////////////////////////////////////////////////////////////////////////
///        class _item_hub
/////////////////////////////////////////////////////////////////////////////////////
item_hub::item_hub()
{
	init();
}

item_hub::~item_hub()
{
	init();
}

void item_hub::init()
{
	int i = 0;
	for(i; i< 12; i++)
	{
		this->iTogo[i] = -1;
		this->iDtmf[i] = -1;
	}
	this->iDtmfCount = 0;
	this->iTogoCount = 0;
	this->iCompDtmfBufNum = -1;
	this->falsegoto = -1;
}

item_hub & item_hub::operator =(const item_hub& h)
{
	int i;
	init();
	this->iCompDtmfBufNum = h.iCompDtmfBufNum;
	this->iTogoCount = h.iTogoCount;
	this->iDtmfCount = h.iDtmfCount;
	for( i = 0; i< h.iDtmfCount; i++)
	{
		this->iDtmf[i] = h.iDtmf[i];
	}
	for( i = 0; i< h.iTogoCount; i ++ )
	{
		this->iTogo[i] = h.iTogo[i];
	}
	this->falsegoto=h.falsegoto;
	return *this;
	//6for(i = 0; i < this->
}

/////////////////////////////////////////////////////////////////////////////////////
///        class _item_ment
/////////////////////////////////////////////////////////////////////////////////////
item_call::item_call()
{
	init();
}

item_call::~item_call()
{
	free();
}

void item_call::init()
{
	std::vector<string>::iterator start,end;
	start = calledParty.begin();
	end = calledParty.end();
	if( start != end )
		this->calledParty.erase(start,end);
	memset(this->callingParty, 0, 64);
	this->enableAutoCall = 0;
	this->enableInCallNoDrop = 0;
	this->falsegoto = -1;
	this->iArgInCount = 0;
	this->defCallDuration = 0;
	memset(this->iArgIn, 0, 10*sizeof(int));
//	callinoutFree = 0;
	this->BusyRetryCount = 0;
	this->BusyRetryPoint = -1;
	this->enableBusyRetry = 0;
}

void item_call::free()
{
	init();
}

item_call &item_call::operator =(const item_call &d)
{
	item_call * pItemCall = (item_call *)&d;
	std::vector<std::string>::iterator start,end,cur;
	start = this->calledParty.begin();
	end = this->calledParty.end();
	if( start != end )
		this->calledParty.erase(start,end);
	start = pItemCall->calledParty.begin();
	end = pItemCall->calledParty.end();
	for( cur = start; cur < end; cur++)
	{
		this->calledParty.push_back(*cur);
	}
	wsprintf(this->callingParty, "%s", d.callingParty);
	memcpy(this->iArgIn, d.iArgIn, 10*sizeof(int));
	this->enableAutoCall = d.enableAutoCall;
	this->enableInCallNoDrop = d.enableInCallNoDrop;
	this->falsegoto = d.falsegoto;
	this->iArgInCount = d.iArgInCount;
	this->defCallDuration = d.defCallDuration;
//	this->callinoutFree = d.callinoutFree;
	this->enableBusyRetry = d.enableBusyRetry;
	this->BusyRetryCount = d.BusyRetryCount;
	this->BusyRetryPoint = d.BusyRetryPoint;
	return *this;
}

int item_call::get_calledParty(int count, char *phoneNum)
{
	std::vector<string>::iterator start,end;
	int iSize;
	iSize = this->calledParty.size();
	start = this->calledParty.begin();
	end = this->calledParty.end();
	if( count >= iSize )
		return 0;
	wsprintf(phoneNum, "%s", (start+count)->c_str());
	return (start+count)->size();
}

void item_call::push_calledParty(char *phoneNum)
{
	std::vector<string>::iterator start,end,cur;
	start = this->calledParty.begin();
	end = this->calledParty.end();
	cur = std::find(start,end,phoneNum);
	if( cur == end )
	{
		this->calledParty.push_back(phoneNum);
	}
}

void item_call::earse_calledParty(char *phoneNum)
{
	std::vector<string>::iterator start,end,cur;
	start = this->calledParty.begin();
	end = this->calledParty.end();
	cur = std::find(start,end,phoneNum);
	if( cur != end )
	{
		this->calledParty.erase(cur);
	}
}

int item_call::get_calledPartySize()
{
	return this->calledParty.size();
}

// ignore this function.. not using..
void item_call::pop_calledParty(char *phoneNum)
{
	return;
}

/////////////////////////////////////////////////////////////////////////////////////
///        class _item_ment
/////////////////////////////////////////////////////////////////////////////////////
item_time_chk::item_time_chk()
{
	init();
}

item_time_chk::~item_time_chk()
{
	free();
}

void item_time_chk::init()
{
	this->bDailyChk = 0;
	this->bSepecialDayChk = 0;
	this->iChk_go = -1;
	this->iDailyChk_go =-1;
	memset(this->dfname, 0, 128);
	memset(this->sfname, 0, 128);
	//this->beginHour = 0;
	//this->endHour = 0;
	//this->beginMin = 0;
	//this->endMin = 0;
}

void item_time_chk::free()
{
	init();
}

item_time_chk &item_time_chk::operator = (const item_time_chk & o)
{
	this->bDailyChk = o.bDailyChk;
	this->bSepecialDayChk = o.bSepecialDayChk;
	this->iChk_go = o.iChk_go;
	sprintf(this->sfname, "%s", o.sfname);
	sprintf(this->dfname, "%s", o.dfname);
	//this->beginHour = o.beginHour;
	//this->endHour = o.endHour;
	//this->beginMin = o.beginMin;
	//this->endMin = o.endMin;
	this->iDailyChk_go = o.iDailyChk_go;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////
///        class _item_ment
/////////////////////////////////////////////////////////////////////////////////////
item_ment::item_ment()
{
	init();
}

item_ment::~item_ment()
{
	free();
}

item_ment	&item_ment::operator=(const item_ment &d)
{
	this->iMentCount = d.iMentCount;
	for(int i = 0; i< d.iMentCount; i++)
	{
		wsprintf(this->szMent[i],d.szMent[i]);
	}
	this->iResetUserInput = d.iResetUserInput;
	return *this;
}
// function
void item_ment::init()
{
	this->iMentCount = 0;
	for(int i = 0; i< 9; i++)
	{
		memset(this->szMent[i],0, 180);
	}
	this->iResetUserInput = -1;
}
void item_ment::free()
{
	init();
}


/////////////////////////////////////////////////////////////////////////////////////
///        class item_uinput
/////////////////////////////////////////////////////////////////////////////////////
item_uinput::item_uinput()
{
	init();
}

item_uinput::~item_uinput()
{
	free();
}

item_uinput &item_uinput::operator=(const item_uinput &d)
{
	this->input_length = d.input_length;
	this->star_key = d.star_key;
	this->sharp_key = d.sharp_key;
	this->iTime = d.iTime;
	this->iInput = d.iInput;
	this->iSharp_go = d.iSharp_go;
	this->iStar_go = d.iStar_go;
	this->iDirect = d.iDirect;
	this->iRefresh = d.iRefresh;
	return *this;
}

// function
void item_uinput::init()
{
	this->input_length = 0;
	this->star_key = 0;
	this->sharp_key = 0;
	this->iTime = 0;
	this->iInput = 0;
	this->iSharp_go = 0;
	this->iStar_go = 0;
	this->iDirect = 0;
	this->iRefresh = 0;
}
void item_uinput::free()
{
	init();
}

/////////////////////////////////////////////////////////////////////////////////////
///        class item_function
/////////////////////////////////////////////////////////////////////////////////////
item_function::item_function()
{
	init();
}

item_function::~item_function()
{
	free();
}

// function
void item_function::init()
{
	this->iDirect = 0;
	this->iMaxSize = 0;
	this->iStopUserInput = 0;
	this->falsegoto = -1;
	this->retrial = 0;
	this->sub_type = -1;
	memset(this->dll_name , 0, 128);
	memset(this->iArgIn, 0, sizeof(int)*10);
	for( int i = 0; i < 6; i++)
	{
		memset(this->szArgIn[i], 0, 48);
	}
	this->iArgInCount = 0;
	memset(this->iArgOut, 0, sizeof(int)*9);
	this->iArgOutCount = 0;
	memset(function_name, 0, 64);
	this->iStopUserInput = -1;
	this->iDirect = -1;
	this->iTimerValue = 0;
	this->iDirection = 0;
	
}
void item_function::free()
{
	init();
}

item_function	&item_function::operator=(const item_function &d)
{
	int i;
	//sub_item.function.iMaxSize = 1024; // 1k byte
	//sub_item.function.iStopUserInput = 13; // none;
	this->iDirect = d.iDirect;
	this->iMaxSize = d.iMaxSize;
	this->iStopUserInput = d.iStopUserInput;
	this->falsegoto = -1;
	//this->iArgInCount = 0;
	this->retrial = 0;
	this->iDirection = d.iDirection;
	wsprintf(this->dll_name,d.dll_name);
	this->falsegoto = d.falsegoto;
	wsprintf(this->function_name, d.function_name);
	
	this->iArgInCount = d.iArgInCount;
	for( i = 0; i< this->iArgInCount; i++)
	{
		this->iArgIn[i] = d.iArgIn[i];
	}
	this->iArgOutCount = d.iArgOutCount;
	for( i = 0; i< this->iArgOutCount; i++)
	{
		this->iArgOut[i] = d.iArgOut[i];
	}
	this->retrial = d.retrial;
	this->sub_type = d.sub_type;
	for( i = 0; i < 6 ; i++)
	{
		//memset( this->szArgIn[i], 0, 48);
		wsprintf(this->szArgIn[i], 
			d.szArgIn[i]);
	}
	this->iTimerValue = d.iTimerValue;
	//this->iStopUserInput = d.iStopUserInput;
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////
///        class SCENARIO_STEP
/////////////////////////////////////////////////////////////////////////////////////
SCENARIO_STEP::SCENARIO_STEP()
{
}

SCENARIO_STEP::~SCENARIO_STEP()
{
	this->free();
}

void SCENARIO_STEP::free()
{
}

void SCENARIO_STEP::Init()
{
    type =-1;
	target_count=0;
}

void SCENARIO_STEP::IncTargetCount()
{
	target_count++;
}

void SCENARIO_STEP::SetTargetID(int i)
{
	target_id[target_count] = i;
}

int	SCENARIO_STEP::operator>(const int &s)
{
	if( this->id > s )
		return 1;
	return 0;
}
int	SCENARIO_STEP::operator<(const int &s)
{
	if( this->id < s )
		return 1;
	return 0;
}
int	SCENARIO_STEP::operator==(const int &s)
{
	if( this->id == s )
		return 1;
	return 0;
}

int	SCENARIO_STEP::operator>(const SCENARIO_STEP &s)
{
	if( this->id > s.id )
		return 1;
	return 0;
}
int	SCENARIO_STEP::operator<(const SCENARIO_STEP &s)
{
	if( this->id < s.id)
		return 1;
	return 0;
}
int	SCENARIO_STEP::operator==(const SCENARIO_STEP &s)
{
	if( this->id == s.id )
		return 1;
	return 0;
}

SCENARIO_STEP &SCENARIO_STEP::operator=(const SCENARIO_STEP &s)
{
	int i = 0;
	id = s.id;
	type = s.type;
	target_count = s.target_count;
	//memset(&target_id, 0, sizeof(int)*10);
	for( i = 0; i< s.target_count; i++)
	{
		target_id[i] = s.target_id[i];
	}
	switch( type )
	{
	case 0:
		{
			this->sub_item.ment = s.sub_item.ment;
		}
		break;
	case 1:
		{
			this->sub_item.uinput = s.sub_item.uinput;
		}
		break;
	case 2:
		{
			this->sub_item.function = s.sub_item.function;
		}
		break;
	case 3:
		{
			this->sub_item.call = s.sub_item.call;
		}
		break;
	case 4:
		{
			this->sub_item.function = s.sub_item.function;
		}
		break;
	case 6:
		{
			this->sub_item.hub = s.sub_item.hub;
		}
		break;
	case 7:
		{
			this->sub_item.function = s.sub_item.function;
		}
		break;
	case 10:
		{
			this->sub_item.time = s.sub_item.time;
		}
		break;
	default:
		break;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////
///        class SStack
/////////////////////////////////////////////////////////////////////////////////////
SStack::SStack()
{
	init();
}

SStack::~SStack()
{
    free();
}

void SStack::free()
{
	this->allocedBufNum = 0;
	this->iItemCount = 0;
	this->deleteAllData();
}

void SStack::init()
{
	this->s_page = -1;
	memset(this->szFile, 0, 256);
	this->iItemCount = 0;
	allocedBufNum = 0;
	iItemCount = 0;
	this->allocedBufNum = 0;
}

SStack & SStack::operator =(const SStack & st)
{
	return *this;
}

int	SStack::equal(SStack &st)
{
	return memcmp(this->szFile, st.szFile, strlen(st.szFile));
}

int SStack::operator >(const SStack &st)
{
	if( this->equal((SStack)st) > 0)
		return 1;
	return 0;
}

int SStack::operator <(const SStack &st)
{
	if( this->equal((SStack)st) < 0)
		return 1;
	return 0;
}

int SStack::operator ==(const SStack &st)
{
	if( !this->equal((SStack)st) )
		return 1;
	return 0;
}

int	SStack::insertData(SCENARIO_STEP & s)
{
	SCENARIO_STEP t;
	t = s;
	this->mStackList.push_back(t);
	return 1;
}

void SStack::deleteAllData()
{
	deleteData();
}

void SStack::deleteData(int iNum)
{
	std::vector<SCENARIO_STEP>::iterator start,end,fdata;
	start = this->mStackList.begin();
	end = this->mStackList.end();
	if( iNum >= 0 )
	{
		fdata = std::find(start,end, iNum);
		if( fdata != end )
		{
			this->mStackList.erase(fdata);
		}
	}
	else
	{
		this->mStackList.erase(start,end);
	}
}

int SStack::getData(int iNum,SCENARIO_STEP &s)
{
	std::vector<SCENARIO_STEP>::iterator start,end,fdata;
	start = this->mStackList.begin();
	end = this->mStackList.end();
	if( !iNum )
	{
		if( start != end )
		{
			s = *start;
			return 1;
		}
	}
	else
	{
		fdata = std::find(start,end, iNum);
		if( fdata != end )
		{
			s = *fdata;
			return 1;
		}
	}
	return 0;
}

int SStack::getDataByOrder(int iCount,SCENARIO_STEP &s)
{
	int iSize;
	std::vector<SCENARIO_STEP>::iterator start,end,fdata;
	start = this->mStackList.begin();
	end = this->mStackList.end();
	iSize = this->mStackList.size();
	if( iSize > iCount && iCount >= 0)
	{
		fdata = start+iCount;
		s = *fdata;
		return 1;
	}
	return 0;
}

void SStack::sortData()
{
	std::vector<SCENARIO_STEP>::iterator start,end;
	start = this->mStackList.begin();
	end = this->mStackList.end();
	if( start != end )
	{
		std::sort(start,end);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
///        end of file..
/////////////////////////////////////////////////////////////////////////////////////
