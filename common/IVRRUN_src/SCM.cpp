// SCM.cpp: implementation of the CSCM class.
//
//////////////////////////////////////////////////////////////////////

// 2003 10 23 : 키입력에서 #이나 *를 일반 문자로 인식하는 부분의 오류를 수정

#include "stdafx.h"
#include "SCM.h"
#include "operate.h"
#include "radmiscfunction.h"
#include "resource.h"
#include "ivrkeydata.h"
#include "voice.h"
#include "SystemBaseSet.h"
#include "msg_define.h"
#include "plugin.h"
#include "bistime.h"
#include <time.h>

#include "cm_strptime.h"
#include "sky_rtp.h"

#include "operate_scenario.h"


#define __TEST__
void WriteLog(char * filename, int a, char * c, char * b);
//void WriteLog2(char * filename, int a, char * c, char * b);
#define WriteLog2(a,b,c,d) WriteLog(a,b,c,d)

/////////////////////////////////////////////////////////////////////////////
// SCM
int RtpPlay(int iCodec, 
			CONNECTION &pCon, 
			char * szFile, 
			char * sLanguage, 
			int & iState, 
			int iSkipState, 
			int &iReserved);

/////////////////////////////////////////////////////////////////////////////
// SCM message handlers

/////////////////////////////////////////////////////////////////////////////
// SCM Message Handler
//
//_tag_SCM::_tag_SCM()
SCM::SCM()
{
	init();
}

SCM::~SCM()
{
	init();
}

void SCM::init()
{
	iProcState = 0;
	m_bCon = FALSE;
	iLanguage = 0;
	m_pCon = 0;
	iCurPage = 0;
	iStepState = 0;
	iScenarioPageSum = 0;
	iCallNum= 0;
	iInstalledLang = 0;
	bTimeLocked = FALSE;
	bUserPressedKey = FALSE;
	bUserInputIgnore = FALSE;
	iReadCount = 0;
	bCallForwardEnabled = 0;
	this->iCurStep = 0;
	this->iRetryBusyCount = -1;
	this->bEnbleBusyRetry = 0;
}


SCM & SCM::operator = (const SCM & scm )
{
	this->bCallForwardEnabled = scm.bCallForwardEnabled;
	this->billingPacket = scm.billingPacket;
	this->bTimeLocked = scm.bTimeLocked;
	this->bUserInputIgnore = scm.bUserInputIgnore;
	this->bUserPressedKey = scm.bUserPressedKey;
	this->iCallNum = scm.iCallNum;
	this->iCurPage = scm.iCurPage;
	this->iCurStep = scm.iCurStep;
	this->iInstalledLang = scm.iInstalledLang;
	this->iLanguage = scm.iLanguage;
	this->iProcState = scm.iProcState;
	this->iReadCount = scm.iReadCount;
	//this->iRetrial = scm.iRetrial;
	this->iRetrialVal = scm.iRetrialVal;
	this->iRetrialCurrentVal = scm.iRetrialCurrentVal;
	this->iScenarioPageSum = scm.iScenarioPageSum;
	this->iStepState = scm.iStepState;
	this->m_bCon = scm.m_bCon;
	this->m_pCon = scm.m_pCon;
	this->bEnbleBusyRetry = scm.bEnbleBusyRetry;
	this->iRetryBusyCount = scm.iRetryBusyCount;
	//this->m_stack_list = scm.m_stack_list;
	return *this;
}

int SCM::operator <(const SCM &scm)
{
	if( this->iCallNum < scm.iCallNum )
		return 1;
	return 0;
}

int SCM::operator >(const SCM &scm)
{
	if( this->iCallNum > scm.iCallNum )
		return 1;
	return 0;
}

int SCM::operator ==(const SCM &scm)
{
	if( this->iCallNum == scm.iCallNum )
		return 1;
	return 0;
}


BOOL GetStopSignal(int iCallNum, int iPosition)
{
	int iILen = 0, iInitPos=0, iCurPos=0;
	int iWillReadLen = 0; // 버퍼에서 읽을 데이터의 길이
	char ttbuf[10];
	BOOL	bAny=FALSE;
	BOOL	bReturn=FALSE;
	// Set Timeout value;
	SCM * this_scm = &scm[iCallNum];
	int iposInput = iPosition;
	iCurPos = this_scm->iReadCount;
	//SCENARIO_STEP * step = &this_scm->m_stack_list[this_scm->iCurPage].m_stack_list[iposInput];
	SCENARIO_STEP step;
	if( GetScenarioItem(this_scm->iCurPage, this_scm->iCurStep, step) )
	{
		iWillReadLen = 1;
		if( step.sub_item.function.iStopUserInput >=0 && step.sub_item.function.iStopUserInput <= 9)
		{
			wsprintf(ttbuf, "%d", step.sub_item.function.iStopUserInput);
		}
		else if( step.sub_item.function.iStopUserInput == 10 )
		{
			wsprintf(ttbuf, "*");
		}
		else if( step.sub_item.function.iStopUserInput == 11 )
		{
			wsprintf(ttbuf, "#");
		}
		else if( step.sub_item.function.iStopUserInput == 12 )
		{
			bAny = TRUE;
		}
		else if( step.sub_item.function.iStopUserInput == 13 )
		{
			// 13으로 지정된 경우 사용자가 전화를 끊기 전까지 기록이 계속된다.
			// 따라서 이 값을 지정할 경우, 주의 해야 한다. 
			// 무한 루프에 빠질 수 있다.
			return FALSE;
		}
		// Search # or *
		if( !this_scm->m_pCon->bUsedThisCall)
			return FALSE;
		//if( !scm->bUserPressedKey )
		//	return FALSE;
		//scm->bUserPressedKey = FALSE;
		if( bAny )
			return TRUE;
		iILen = strlen(this_scm->szInput);
		iCurPos = this_scm->iReadCount;
		for( ; iCurPos < iILen; iCurPos++)
		{
			//update : 2003 10 23 13 53
			//#define _KEY_DEBUG
#ifdef _KEY_DEBUG
			WriteLog("uinput", iCallNum,"input", (char*)&szInput[iReadCount]);
			char tbuf[124];
			memset(tbuf, 0, 124);
			memcpy(tbuf, (char*)&szInput[iReadCount], iWillReadLen);
			WriteLog("uinput", iCallNum,"input", tbuf);
#endif
			this_scm->iReadCount++;
			if( memcmp((char *)&this_scm->szInput[iCurPos], ttbuf, 1) == NULL )
			{
				bReturn = TRUE;
				break;
			}
		}
		return bReturn;
	}
	return FALSE;
}

BOOL GetVoiceRecStopSignal(int iCallNum, int iPosition)
{
	return GetStopSignal(iCallNum, iPosition);
}

// 시나리오 진행 중 사용자의 입력 버퍼에서 필요한 만큼의 데이터를 가져와서 시나리오를 진행함.
// 사용자 입력과 관련된 모든 이벤트를 진행함.
// return value: 
// -1 : Scenario exit
// 0  : go to next target
// 1  : don't go to next target
//#define _KEY_DEBUG
int OnEventUserInput(int iCallNum, int iPosition,HANDLE hEvent)
{
	int iILen = 0, iInitPos=0, iCurPos=0;
	int iWillReadLen = 0; // 버퍼에서 읽을 데이터의 길이
	int iSharp = 0;
	int iStar = 0;
	int iposInput= 0;
	int iKeyType = 0;
	int iKeyValue = 0;
	int iKeyGo = 0;
	int iPressedKey=0;
	// Set Timeout value;
	int iWatingCount = 0;
	int iStep = 0, iLimitLen = 0;
	SCM * this_scm = &scm[iCallNum];
	BOOL bEndKeyPressed = FALSE;
	
	iposInput = iPosition;
	iCurPos = this_scm->iReadCount;
	//SCENARIO_STEP * step = &this_scm->m_stack_list[this_scm->iCurPage].m_stack_list[iposInput];
	SCENARIO_STEP step;
	if( GetScenarioItem(this_scm->iCurPage, this_scm->iCurStep, step) )
	{
		//	memset(this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
		iWillReadLen = step.sub_item.uinput.input_length;
		
		iStep = (iInputWaitTime *1000)/ 18;
		
		// Search # or *
		while( 1)
		{
			if( !this_scm->m_pCon->bUsedThisCall || this_scm->m_pCon->bCallReusing)
			{
				return -1;
			}
			iILen = strlen(this_scm->szInput);
			iCurPos = this_scm->iReadCount;
			for( ; iCurPos < iILen; iCurPos++)
			{
				if( memcmp((char *)&this_scm->szInput[iCurPos], "#", 1) == NULL )
				{
					iSharp = 1;
					bEndKeyPressed = TRUE;
				}
				if( memcmp((char *)&this_scm->szInput[iCurPos], "*", 1) == NULL )
				{
					iStar = 1;
					bEndKeyPressed = TRUE;
				}
			}
			if( bEndKeyPressed == TRUE )
			{
				iWillReadLen =  iCurPos - this_scm->iReadCount;
				//WriteLog("h245", iCurPos, "H245(******)", szInput);
				BOOL bFirstInput = FALSE;
				
				// 0-exit 1-return 2-character 3-back
				iPressedKey = 0;
				iKeyValue = 2;
				if( iSharp)
				{
					iPressedKey = iSharp;
					iKeyType = 0;
					iKeyValue = step.sub_item.uinput.sharp_key;
					iKeyGo = step.sub_item.uinput.iSharp_go;
					
					if( iKeyValue != 2 )
						if( this_scm->szInput[this_scm->iReadCount] == '#' )
							bFirstInput = TRUE;
				}
				else if( iStar )
				{
					iPressedKey = iStar;
					iKeyType = 1;
					iKeyValue = step.sub_item.uinput.star_key;
					iKeyGo = step.sub_item.uinput.iStar_go;
					
					if( iKeyValue != 2 )
						if( this_scm->szInput[this_scm->iReadCount] == '*' )
							bFirstInput = TRUE;
				}
				
				if( iPressedKey == 1)
				{
					switch( iKeyValue )
					{
					case 3:// back
						//
						this_scm->bUserInputIgnore = TRUE;
						this_scm->iReadCount = 0;
						memset( this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
						bEndKeyPressed = FALSE;
						if( iKeyGo >= 0 )
						{
							this_scm->iCurStep = iKeyGo;
						}
						else
						{
							this_scm->iCurStep = 0;
						}
						this_scm->iStepState = 0;
						this_scm->iRetrialVal = 0;
						//this_scm->iRetrial = 0;
						this_scm->bUserInputIgnore = FALSE;
						return 1;
					case 1:// return
						if( bFirstInput )
						{
							this_scm->iReadCount++;
							bEndKeyPressed = FALSE;
							//pDlg->WriteToConsole(iCallNum, "H245(break)", "1");
							this_scm->iStepState = 0;
						}
						else
						{
							iWillReadLen--;
							goto endLoop;
						}
						break;
					case 0:// exit, calldrop
					case 2:// character
						break;
					}
				}
			}
			
			// 사용자의 입력이 필요한 데이터의 최대 크기만큼 진행된 경우...
			if( step.sub_item.uinput.input_length <= ( iILen - this_scm->iReadCount ) && bEndKeyPressed == FALSE)
			{
				iWillReadLen = step.sub_item.uinput.input_length;
				goto endLoop;
			}
			
			//사용자의 입력이 있은 후, n sec 후에도 입력이 없는 경우를 검사함.
			if( this_scm->bUserPressedKey )
			{
				if( iWatingCount++ < iStep)
				{
					//this_scm->bUserPressedKey = TRUE;
#if 1
					if( hEvent )
						WaitForSingleObject( hEvent, 5 );
#else
					Sleep(8);
#endif
				} 
				else
				{
					iWillReadLen = iILen - this_scm->iReadCount; //
					goto endLoop;
				}
			}
			else
			{
				this_scm->bUserPressedKey = TRUE;
//#if 1
//				if( hEvent )
//					WaitForSingleObject( hEvent, 1 ); 
//#else
//				Sleep(1);
//#endif
				iWatingCount = 0;
			}
		}
endLoop:
		//update : 2003 10 23 13 53
		//#define _KEY_DEBUG
#ifdef _KEY_DEBUG
		WriteLog("uinput", iCallNum,"input", (char*)&szInput[iReadCount]);
		char tbuf[124];
		memset(tbuf, 0, 124);
		memcpy(tbuf, (char*)&szInput[iReadCount], iWillReadLen);
		WriteLog("uinput", iCallNum,"input", tbuf);
#endif
		if(step.sub_item.uinput.iInput>=5&&step.sub_item.uinput.iInput<= 9)
		{
			if(iWillReadLen >= 48)
				iLimitLen = 48;
			else
				iLimitLen = iWillReadLen;
			memset(this_scm->szArgIn[step.sub_item.uinput.iInput-5], 0, 48);
			if( iLimitLen > 0 )
				memcpy(this_scm->szArgIn[step.sub_item.uinput.iInput-5],
					(char*)&this_scm->szInput[this_scm->iReadCount], iLimitLen);
		}
		else if (step.sub_item.uinput.iInput == 1) // Other party TelNumber
		{
			if(iWillReadLen >= 20)
				iLimitLen = 20;
			else
				iLimitLen = iWillReadLen;
			
			memset(this_scm->billingPacket.pBody.Destination, 0x20, 20 );
			memcpy(this_scm->billingPacket.pBody.Destination, (char*)&this_scm->szInput[this_scm->iReadCount], iLimitLen);
		}
		else if (step.sub_item.uinput.iInput == 11) // user pin number
		{ 
			if(iWillReadLen >= 20)
				iLimitLen = 20;
			else
				iLimitLen = iWillReadLen;
			
			memset( this_scm->billingPacket.pBody.Pin, 0x20 , 20 ); 
			memcpy(this_scm->billingPacket.pBody.Pin, (char*)&this_scm->szInput[this_scm->iReadCount], iLimitLen);
		}
		
		this_scm->iStepState = 0;
		this_scm->iReadCount = iWillReadLen + this_scm->iReadCount;
		
		//updated 2003 10 23 1 33
		if( step.sub_item.uinput.sharp_key != 2)
		{
			if( this_scm->szInput[this_scm->iReadCount] == '#')
				this_scm->iReadCount++;
		}
		if(	step.sub_item.uinput.star_key != 2)
		{
			if( this_scm->szInput[this_scm->iReadCount] == '*')
				this_scm->iReadCount++;
		}
	}
	return 0;
}
#if 0
int DailyChk(VOID *pvTime)
{
	time_t tt;
	item_time_chk *pTime = (item_time_chk *) pvTime;
	if( !pTime )
		return 0;
	struct tm *current;
	time(&tt);
	current = localtime(&tt);
	if( current->tm_hour < pTime->beginHour )
	{
		return 1;
	}
	if(current->tm_hour == pTime->beginHour)
	{
		if( current->tm_min < pTime->beginMin )
			return 1;
	}
	if( current->tm_hour > pTime->endHour)
	{
		return 1;
	}
	if( current->tm_hour == pTime->endHour )
	{
		if( current->tm_min > pTime->endMin )
			return 1;
	}
	return 0;
}
#else
int DailyChk(char *fName)
{
	time_t tt;
	FILE *fp=NULL;
	struct tm tTime, *current;
	//struct tm tTimeStart,tTimeEnd;
	CBISTIME start,end,cur;
	
	char tbuf[64];
	if( fName ==NULL)
		return 0;
	if( strlen( fName ) == 0)
		return 0;
	fp = fopen(fName, "r");
	if( !fp )
	{
		return 0;
	}
	
	time(&tt);
	current = localtime(&tt);
	cur = *current;
	
	memset(tbuf, 0, 64);
	while(fgets(tbuf, 64, fp))
	{
		if( !memcmp(tbuf, "BEGIN:", 6) )
		{
			strptime(&tbuf[6], "%H %M %S", &tTime);
			start = tTime;
		}
		else if( !memcmp(tbuf, "END:", 4 ))
		{
			strptime(&tbuf[4], "%H %M %S", &tTime);
			end = tTime;
		}
		memset(tbuf, 0, 64);
	}
	fclose(fp);
	if( start.t.tm_hour <= cur.t.tm_hour && 
		end.t.tm_hour >= cur.t.tm_hour )
	{

	}

	//if( current->tm_hour < pTime->beginHour )
	if( cur.t.tm_hour < start.t.tm_hour )
	{
		return 1;
	}
	
	//if(current->tm_hour == pTime->beginHour)
	if( cur.t.tm_hour == start.t.tm_hour )
	{
		//if( current->tm_min < pTime->beginMin )
		if( cur.t.tm_min < start.t.tm_min)
			return 1;
	}
	
	//if( current->tm_hour > pTime->endHour)
	if( cur.t.tm_hour > end.t.tm_hour )
	{
		return 1;
	}
	//if( current->tm_hour == pTime->endHour )
	if( cur.t.tm_hour == end.t.tm_hour)
	{
		//if( current->tm_hour > pTime->endMin )
		if( cur.t.tm_min > end.t.tm_min )
			return 1;
	}
	return 0;
}
#endif

int	SpecialDayChk(char *fName)
{
	time_t tt;
	FILE *fp=NULL;
	struct tm tTime, *current;
	CBISTIME start,end,cur;
	
	char tbuf[64];

	//fp = fopen("blocktime.ini", "r");
	if( fName == NULL )
		return 0;
	if( strlen( fName ) == 0 )
		return 0;
	fp = fopen(fName, "r");
	if( !fp )
	{
		return 0;
	}
	
	time(&tt);
	current = localtime(&tt);
	cur = *current;
	
	memset(tbuf, 0, 64);
	while(fgets(tbuf, 64, fp))
	{
		if( !memcmp(tbuf, "BEGIN:", 6) )
		{
			strptime(&tbuf[6], "%Y %m %d %H %M %S", &tTime);
			start = tTime;
		}
		else if( !memcmp(tbuf, "END:", 4 ))
		{
			strptime(&tbuf[4], "%Y %m %d %H %M %S", &tTime);
			end = tTime;
		}
		memset(tbuf, 0, 64);
	}
	fclose(fp);
	if( start < cur && cur < end )
		return 1;
	
	return 0;
}

DWORD WINAPI ScenarioRunner(PVOID pArg)
{
	SCM * this_scm = (SCM * ) pArg;
	this_scm->m_bCon = TRUE;
	//SCENARIO_STEP * step;
	SCENARIO_STEP step;
	
	int iRetcode = 0;
	int iposMent =0 ;
	int iposInput = 0;
	int ic = 0;

	char tbuf[32];
	BOOL iFirst = TRUE;
	if( this_scm->iScenarioPageSum == 0)
	{
		//if( hEvent )
		//{
		//	CloseHandle( hEvent );
		//	hEvent = NULL;
		//}
		this_scm->m_bCon = FALSE;
		CloseHandle( pCon[this_scm->iCallNum].thread_sce);
		pCon[this_scm->iCallNum].thread_sce = NULL;
		return 1;
	}
	wsprintf(tbuf,"Event-%03d", this_scm->iCallNum);
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, tbuf );
	
	this_scm->iReadCount = 0;
	memset(this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
	this_scm->iRetryBusyCount = -1;
	this_scm->bEnbleBusyRetry = 0;
	
	this_scm->iRetrialVal = 0;
	//this_scm->iRetrial = 0;
	this_scm->bTimeLocked = FALSE;
	for( ic =0; ic < 5 ; ic++)
	{
		memset( this_scm->szArgIn[ic], 0, 48);
	}
	this_scm->iLanguage = 0;
	if( this_scm->iProcState == 1)
	{
		if( hEvent )
		{
			CloseHandle( hEvent );
			hEvent = NULL;
		}
		this_scm->m_bCon = FALSE;
		CloseHandle( pCon[this_scm->iCallNum].thread_sce);
		pCon[this_scm->iCallNum].thread_sce = NULL;
		return 1;
	}
	/////////////////
	////////////////
	this_scm->bUserPressedKey = FALSE;
	this_scm->iCurPage = 0;
	this_scm->iStepState = 0;
	// fixed at 2003 9 30 13 11 : 1000->MAX_DTMF_INPUT_LENGTH

	if( hEvent )
		WaitForSingleObject( hEvent, 300 ); 

	this_scm->iRetrialVal = 0;
	//this_scm->iRetrial = 0;
	this_scm->iCurStep = 0;
	//this_scm->bInputDirect = 0;
	//this_scm->bUnVoiceRelay = 0;
	for( ; ; )
	{
		if( this_scm->iCallNum < 0 && this_scm->iCallNum>=iMaxAllowedCall)
			return 1;
		if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
		{
			scm->iProcState = 2;
			WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
			if( hEvent )
			{
				CloseHandle( hEvent );
				hEvent = NULL;
			}
			this_scm->m_bCon = FALSE;
			CloseHandle( pCon[this_scm->iCallNum].thread_sce);
			pCon[this_scm->iCallNum].thread_sce = NULL;
			return 1;
		}
		if( this_scm->iCurStep != -1 )
		{
			step.Init();
			if(! GetScenarioItem(this_scm->iCurPage, this_scm->iCurStep, step) )
				break;
			//step = &this_scm->m_stack_list[this_scm->iCurPage].m_stack_list[this_scm->iCurStep];
		}
		else if( this_scm->iCurStep == -1 )
		{
//			if( pCon[this_scm->iCallNum].bUsedThisCall )
//				PostMessage( m_hWnd,WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
			break;
		}
		if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
		{
			scm->iProcState = 2;
			WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
			if( hEvent )
			{
				CloseHandle( hEvent );
				hEvent = NULL;
			}
			this_scm->m_bCon = FALSE;
			CloseHandle( pCon[this_scm->iCallNum].thread_sce);
			pCon[this_scm->iCallNum].thread_sce = NULL;
			return 1;
		}

		if( this_scm->iCurStep == 0 )
		{
			this_scm->iLanguage = 0;
		}
		if( this_scm->iCallNum >= iMaxAllowedCall)
		{
			scm->iProcState = 2;
			WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
			if( hEvent )
			{
				CloseHandle( hEvent );
				hEvent = NULL;
			}
			this_scm->m_bCon = FALSE;
			CloseHandle( pCon[this_scm->iCallNum].thread_sce);
			pCon[this_scm->iCallNum].thread_sce = NULL;
			return 1;
		}

		{
			if( !pCon[this_scm->iCallNum].bUsedThisCall )
			{
				scm->iProcState = 2;
				WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point2");
				if( hEvent )
				{
					CloseHandle( hEvent );
					hEvent = NULL;
				}
				this_scm->m_bCon = FALSE;
				CloseHandle( pCon[this_scm->iCallNum].thread_sce);
				pCon[this_scm->iCallNum].thread_sce = NULL;
				return 1;
			}
			else
			{
				char tbuf[32];
				sprintf(tbuf, "Item - %d/%d", step.type , step.id );
				WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", tbuf);
			}
		}
		if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
		{
			scm->iProcState = 2;
			WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
			if( hEvent )
			{
				CloseHandle( hEvent );
				hEvent = NULL;
			}
			this_scm->m_bCon = FALSE;
			CloseHandle( pCon[this_scm->iCallNum].thread_sce);
			pCon[this_scm->iCallNum].thread_sce = NULL;
			return 1;
		}
	
		//if( !step)
		//	break;
		if( step.type < 0 )
		{
			scm->iProcState = 2;
			WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
			if( hEvent )
			{
				CloseHandle( hEvent );
				hEvent = NULL;
			}
			this_scm->m_bCon = FALSE;
			CloseHandle( pCon[this_scm->iCallNum].thread_sce);
			pCon[this_scm->iCallNum].thread_sce = NULL;
			return 1;
		}

		if( step.target_count <= 0)
			this_scm->iCurStep = GetScenarioCount(this_scm->iCurPage);
		switch( step.type)
		{
		case 0: // ment play
			{
				iposMent = this_scm->iCurStep;
				if( step.sub_item.ment.iResetUserInput == 1)
				{
					this_scm->iStepState = 0;
					this_scm->iReadCount = 0;
					memset( this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
				}
				else
				{
					if( this_scm->iStepState == 2 )
						goto break_ment;
				}
				for( int i = 0; i < step.sub_item.ment.iMentCount; i++)
				{
					if( !pCon[this_scm->iCallNum].bOpendChannel )
						goto end_of_loop;
					if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing ||
						!pCon[this_scm->iCallNum].bOpendChannel )
					{
						scm->iProcState = 2;
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						if( hEvent )
						{
							CloseHandle( hEvent );
							hEvent = NULL;
						}
						this_scm->m_bCon = FALSE;
						CloseHandle( pCon[this_scm->iCallNum].thread_sce);
						pCon[this_scm->iCallNum].thread_sce = NULL;
						return 1;
					}	
					if( this_scm->iStepState == 2 )
						goto break_ment;
					iRetcode = RtpPlay(pCon[this_scm->iCallNum].iChannelCodec,pCon[this_scm->iCallNum], 
						step.sub_item.ment.szMent[i],
						sLanguage[this_scm->iLanguage],
						this_scm->iStepState, 0, this_scm->bUserInputIgnore);
					//if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing ||
					//	!pCon[this_scm->iCallNum].bOpendChannel )
					//{
					//	scm->iProcState = 2;
					//	WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
					//	if( hEvent )
					//	{
					//		CloseHandle( hEvent );
					//		hEvent = NULL;
					//	}
					//	this_scm->m_bCon = FALSE;
					//	CloseHandle( pCon[this_scm->iCallNum].thread_sce);
					//	pCon[this_scm->iCallNum].thread_sce = NULL;
					//	return 1;
					//}
					if( iRetcode == 0 )
					{ // ment stop
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - ment stop");
						goto break_ment;
					}
					else if(iRetcode == -1) // error
					{
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - ment error");
						this_scm->iCurStep = -1;
						goto break_ment;
					}
					else if( iRetcode == 1)
					{	 // normal
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "ment normal");
					}
					// fixed at 2003 9 30 13 11 : 150->100
					if( hEvent )
						WaitForSingleObject( hEvent, 60 ); 
					if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing ||
						!pCon[this_scm->iCallNum].bOpendChannel )
					{
						scm->iProcState = 2;
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						if( hEvent )
						{
							CloseHandle( hEvent );
							hEvent = NULL;
						}
						this_scm->m_bCon = FALSE;
						CloseHandle( pCon[this_scm->iCallNum].thread_sce);
						pCon[this_scm->iCallNum].thread_sce = NULL;
						return 1;
					}
				}
			}
break_ment:
			if( !pCon[this_scm->iCallNum].bUsedThisCall )
				goto end_of_loop;
			if( step.target_count > 0)
			{
				if( step.target_id[0] == 999 )
				{
					PostMessage( m_hWnd,WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
					this_scm->iCurStep = GetScenarioCount(this_scm->iCurPage);
					goto end_of_loop;
				}
				else
				{ // 정상 종료
					if( step.target_id[0] == -1)
					{
#if 0
						goto end_of_loop;
#else
						//WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						PostMessage( m_hWnd,WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
						//this_scm->iCurStep = GetScenarioCount(this_scm->iCurPage);
						goto end_of_loop;
#endif
					}
					else
						SetCurStep( this_scm->iCallNum, step.target_id[0]);
				}
			}
			else 
			{
				PostMessage( m_hWnd,WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
				this_scm->iCurStep = GetScenarioCount(this_scm->iCurPage);
				goto end_of_loop;
			}
			break;
		case 1: // Input String
			{
				int iRetInput = 0;
				//if( m_pCon->iIndicationLength < 0 || m_pCon->iIndicationLength > 48 )
				//	goto end_of_loop;
				
				//////
				//this_scm->bInputDirect = 0;
				//this_scm->bInputDirect = step.sub_item.function.iDirect;
				
				if( step.sub_item.uinput.iRefresh )
				{
					memset(this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH );
					this_scm->iReadCount = 0;
				}
				pCon[this_scm->iCallNum].m_iInputDirection = step.sub_item.uinput.iDirect;
				
				iRetInput = OnEventUserInput(this_scm->iCallNum, this_scm->iCurStep,hEvent);
				if( iRetInput == 0 )
				{
					SetCurStep(this_scm->iCallNum, step.target_id[0]);
				}
				else if( iRetInput == -1 )
				{
					if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
					{
						scm->iProcState = 2;
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						if( hEvent )
						{
							CloseHandle( hEvent );
							hEvent = NULL;
						}
						this_scm->m_bCon = FALSE;
						CloseHandle( pCon[this_scm->iCallNum].thread_sce);
						pCon[this_scm->iCallNum].thread_sce = NULL;
						pCon[this_scm->iCallNum].m_iInputDirection = 0;
						return 1;
					}

				}
				else if( iRetInput == 1 )
				{ // 107..
					; // don't go next step and back or go to some where.
				}
				else
				{
					SetCurStep(this_scm->iCallNum, step.target_id[0]);
				}
				pCon[this_scm->iCallNum].m_iInputDirection = 0;
				if( !pCon[this_scm->iCallNum].bUsedThisCall )
					goto end_of_loop;
			}
			break;
		case 2: // function
			{
				if( this_scm->iStepState == 3)
					break;
				//this_scm->iStepState = 0;
				SStack *st = GetStackPoint(this_scm->iCurPage);
				iRetcode = FunctionManager( this_scm->iCallNum, st, &step, this_scm->iCurStep);

				//memset(LogBuff, 0, 20 );
				//sprintf(LogBuff, "ret:%d iposMent:%d iposInput:%d", iRetcode , iposMent, iposInput );
				//WriteLog2(PROG_NAME, this_scm->iCallNum, "SCM", LogBuff);

				if( iRetcode == -1)
				{//call drop
					PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
					this_scm->iCurStep = GetScenarioCount(this_scm->iCurPage);
					goto end_of_loop;
				}
				else if( iRetcode == 0)
				{ // step hold
					this_scm->iCurStep = iposMent;
				}
				else if( iRetcode == 3)
				{ // step hold
					this_scm->iCurStep = iposInput;
				}
				else if( iRetcode == 4)
				{
					;
				}
				else if( iRetcode == 5)
					goto end_of_loop;
			}
			if( !pCon[this_scm->iCallNum].bUsedThisCall )
				goto end_of_loop;
			break;
		case 3: // Make Call
			{
				int iCount=0,iTelNumCount;
				iTelNumCount = step.sub_item.call.get_calledPartySize();
//				if( step.sub_item.call.callinoutFree)
				{
					if( pCon[this_scm->iCallNum+iMaxAllowedCall].bUsedThisCall )
					{
						goto call_end_routine_1;
					}
				}
				pCon[this_scm->iCallNum].defOutCallDuration = step.sub_item.call.defCallDuration;
				if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
				{
					scm->iProcState = 2;
					WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
					if( hEvent )
					{
						CloseHandle( hEvent );
						hEvent = NULL;
					}
					this_scm->m_bCon = FALSE;
					CloseHandle( pCon[this_scm->iCallNum].thread_sce);
					pCon[this_scm->iCallNum].thread_sce = NULL;
					return 1;
				}
				if( step.sub_item.call.enableInCallNoDrop)
					pCon[this_scm->iCallNum].bInChannelConnect = 1;
				else
					pCon[this_scm->iCallNum].bInChannelConnect = 0;
				if( step.sub_item.call.enableBusyRetry )
					pCon[this_scm->iCallNum].bRetrialCall = 1;
				else
					pCon[this_scm->iCallNum].bRetrialCall = 0;
redial:
				if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
				{
					scm->iProcState = 2;
					WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
					if( hEvent )
					{
						CloseHandle( hEvent );
						hEvent = NULL;
					}
					this_scm->m_bCon = FALSE;
					CloseHandle( pCon[this_scm->iCallNum].thread_sce);
					pCon[this_scm->iCallNum].thread_sce = NULL;
					return 1;
				}
				if( pCon[this_scm->iCallNum+iMaxAllowedCall].iCallState == 4 )
				{
					goto call_end_routine_1;
				}
				if( step.sub_item.call.enableAutoCall )
				{
					char sbuf[64];
					if( step.sub_item.call.enableBusyRetry)
						iCount = 0;					
					if( iCount < iTelNumCount )
					{
						if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
						{
							scm->iProcState = 2;
							WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
							if( hEvent )
							{
								CloseHandle( hEvent );
								hEvent = NULL;
							}
							this_scm->m_bCon = FALSE;
							CloseHandle( pCon[this_scm->iCallNum].thread_sce);
							pCon[this_scm->iCallNum].thread_sce = NULL;
							return 1;
						}
						int len = step.sub_item.call.get_calledParty(iCount++, sbuf);
						if( len )
						{
							memset(this_scm->billingPacket.pBody.Destination, 0x20, 20 );
							memcpy(this_scm->billingPacket.pBody.Destination, sbuf, len);
						}
						else
						{
							if( step.sub_item.call.falsegoto >= 0 )
							{
								SetCurStep(this_scm->iCallNum, step.sub_item.call.falsegoto);
							}
							else
							{
								if( pCon[this_scm->iCallNum].bUsedThisCall )
									PostMessage( m_hWnd,WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
								step.target_count = -1;
							}
							goto call_end_routine_2;
						}
					}
					else
					{
						if( step.sub_item.call.falsegoto >= 0 )
						{
							SetCurStep(this_scm->iCallNum, step.sub_item.call.falsegoto);
						}
						else
						{
							if( pCon[this_scm->iCallNum].bUsedThisCall )
								PostMessage( m_hWnd,WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
							step.target_count = -1;
						}
						goto call_end_routine_2;
					}
				}
				if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
				{
					scm->iProcState = 2;
					WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
					if( hEvent )
					{
						CloseHandle( hEvent );
						hEvent = NULL;
					}
					this_scm->m_bCon = FALSE;
					CloseHandle( pCon[this_scm->iCallNum].thread_sce);
					pCon[this_scm->iCallNum].thread_sce = NULL;
					return 1;
				}
				pCon[this_scm->iCallNum].bOutIsBusy = 0;
				SendMessage(m_hWnd, WM_VOICON_CallMake, this_scm->m_pCon->iCallNumber, 0);
				
				if( step.sub_item.call.enableInCallNoDrop)
				{

					int waitcount=0;
					if( !step.sub_item.call.enableBusyRetry )
					{
						while( pCon[this_scm->iCallNum+iMaxAllowedCall].bUsedThisCall )
						{
							if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
							{
								scm->iProcState = 2;
								WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
								if( hEvent )
								{
									CloseHandle( hEvent );
									hEvent = NULL;
								}
								this_scm->m_bCon = FALSE;
								CloseHandle( pCon[this_scm->iCallNum].thread_sce);
								pCon[this_scm->iCallNum].thread_sce = NULL;
								return 1;
							}
							if( hEvent )
								WaitForSingleObject( hEvent, 100 );
							if( !pCon[this_scm->iCallNum].bUsedThisCall
								|| pCon[this_scm->iCallNum].bCallReusing
								|| !pCon[this_scm->iCallNum].bOpendChannel)
							{
								scm->iProcState = 2;
								WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
								if( hEvent )
								{
									CloseHandle( hEvent );
									hEvent = NULL;
								}
								this_scm->m_bCon = FALSE;
								CloseHandle( pCon[this_scm->iCallNum].thread_sce);
								pCon[this_scm->iCallNum].thread_sce = NULL;
								return 1;
							}
							if( pCon[this_scm->iCallNum+iMaxAllowedCall].iCallState == 4 )
							{
								goto call_end_routine_1;
							}
							//if( pCon[this_scm->iCallNum+iMaxAllowedCall].bOpendChannel )
							//	goto call_end_routine_1;
							if(waitcount < 100 )
							{
								waitcount++;
							}
							else
							{
								if( pCon[this_scm->iCallNum+iMaxAllowedCall].bUsedThisCall )
									PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum+iMaxAllowedCall, (long)cmReasonTypeUndefinedReason);
								if( hEvent )
									WaitForSingleObject( hEvent, 500 );
								goto redial;
							}
						}
						if( /*!pCon[this_scm->iCallNum+iMaxAllowedCall].bOpendChannel 
							||*/!pCon[this_scm->iCallNum+iMaxAllowedCall].iCallState )
						{
							goto redial;
						}
						else
						{
							goto call_end_routine_1;
						}
					}
					else
					{
						if( step.sub_item.call.BusyRetryCount > 0 )
						{
							if( this_scm->iRetryBusyCount < 0 )
							{
								this_scm->iRetryBusyCount = step.sub_item.call.BusyRetryCount;
							}
							
							if( this_scm->iRetryBusyCount > 0 )
							{
								this_scm->iRetryBusyCount --;
								while( pCon[this_scm->iCallNum+iMaxAllowedCall].bUsedThisCall )
								{
									if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
									{
										scm->iProcState = 2;
										WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
										if( hEvent )
										{
											CloseHandle( hEvent );
											hEvent = NULL;
										}
										this_scm->m_bCon = FALSE;
										CloseHandle( pCon[this_scm->iCallNum].thread_sce);
										pCon[this_scm->iCallNum].thread_sce = NULL;
										return 1;
									}

									if( hEvent )
										WaitForSingleObject( hEvent, 100 );
									if( !pCon[this_scm->iCallNum].bOpendChannel)
									{
										goto end_of_loop;							
									}
									if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
									{
										scm->iProcState = 2;
										WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
										if( hEvent )
										{
											CloseHandle( hEvent );
											hEvent = NULL;
										}
										this_scm->m_bCon = FALSE;
										CloseHandle( pCon[this_scm->iCallNum].thread_sce);
										pCon[this_scm->iCallNum].thread_sce = NULL;
										return 1;
									}

									if( pCon[this_scm->iCallNum+iMaxAllowedCall].bOutIsBusy )
									{
										if( pCon[this_scm->iCallNum+iMaxAllowedCall].bUsedThisCall )
											PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum+iMaxAllowedCall, (long)cmReasonTypeUndefinedReason);
										if( hEvent )
											WaitForSingleObject( hEvent, 300 );
										if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
										{
											scm->iProcState = 2;
											WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
											if( hEvent )
											{
												CloseHandle( hEvent );
												hEvent = NULL;
											}
											this_scm->m_bCon = FALSE;
											CloseHandle( pCon[this_scm->iCallNum].thread_sce);
											pCon[this_scm->iCallNum].thread_sce = NULL;
											return 1;
										}
										if( this_scm->iRetryBusyCount> 0)
										{
											SetCurStep(this_scm->iCallNum, step.sub_item.call.BusyRetryPoint);
											goto call_end_routine_2;
										}
										else
										{
											this_scm->iRetryBusyCount = -1;
											if( pCon[this_scm->iCallNum].bUsedThisCall )
												PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
											goto call_end_routine_1;
										}
									}
									if(waitcount < 250 )
									{
										waitcount++;
									}
								}
								if( pCon[this_scm->iCallNum+iMaxAllowedCall].bOutIsBusy )
								{
									if( pCon[this_scm->iCallNum+iMaxAllowedCall].bUsedThisCall )
										PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum+iMaxAllowedCall, (long)cmReasonTypeUndefinedReason);
									if( hEvent )
										WaitForSingleObject( hEvent, 500 );
									if( this_scm->iRetryBusyCount> 0)
									{
										SetCurStep(this_scm->iCallNum, step.sub_item.call.BusyRetryPoint);
										goto call_end_routine_2;
									}
									else
									{
										this_scm->iRetryBusyCount = -1;
										if( pCon[this_scm->iCallNum].bUsedThisCall )
											PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
										goto call_end_routine_1;
									}
								}
								goto  call_end_routine_1;
							}
							else
							{
								this_scm->iRetryBusyCount --;
								if( pCon[this_scm->iCallNum].bUsedThisCall )
									PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
								goto call_end_routine_1;
							}
						}
					}
				}
call_end_routine_1:
				if( step.target_id[0] >= 0)
					SetCurStep(this_scm->iCallNum, step.target_id[0]);
				else
					step.target_count = -1;
call_end_routine_2:
				if( !pCon[this_scm->iCallNum].bUsedThisCall )
					goto end_of_loop;
			}
			break;
#if 0
		case 3: // Input String from out bound.
			{
				int iRetInput = 0;
				//if( m_pCon->iIndicationLength < 0 || m_pCon->iIndicationLength > 48 )
				//	goto end_of_loop;
				//this_scm->bUnVoiceRelay = 1;
				
				////////
				//this_scm->bInputDirect = step.sub_item.function.iDirect;
				
				//memset(this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
				iRetInput = OnEventUserInput(this_scm->iCallNum, this_scm->iCurStep,hEvent);
				//this_scm->bUnVoiceRelay = 0;
				if( iRetInput == 0 )
				{
					SetCurStep(this_scm->iCallNum, step.target_id[0]);
				}
				else if( iRetInput == -1 )
				{
					goto end_of_loop;
				}
				else if( iRetInput == 1 )
				{
					; // don't go next step and back or go to some where.
				}
				if( !pCon[this_scm->iCallNum].bUsedThisCall )
					goto end_of_loop;
			}
			break;
#endif
		case 4: // waiting, sleeping. with noticing stop key.
			////
			////this_scm->bInputDirect = step.sub_item.function.iDirect;
			if( hEvent )
				WaitForSingleObject( hEvent, 500 );
			if( step.sub_item.function.iStopUserInput >=0 &&
				step.sub_item.function.iStopUserInput <= 12 )
			{
				//pCon[this_scm->iCallNum].
				pCon[this_scm->iCallNum].m_iInputDirection = step.sub_item.function.iDirection;
				while( !GetStopSignal(this_scm->iCallNum, this_scm->iCurStep ))
				{
					if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
					{
						scm->iProcState = 2;
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						if( hEvent )
						{
							CloseHandle( hEvent );
							hEvent = NULL;
						}
						this_scm->m_bCon = FALSE;
						CloseHandle( pCon[this_scm->iCallNum].thread_sce);
						pCon[this_scm->iCallNum].thread_sce = NULL;
						pCon[this_scm->iCallNum].m_iInputDirection = 0;
						return 1;
					}
					
					if( !pCon[this_scm->iCallNum].bOpendChannel )
					{
						pCon[this_scm->iCallNum].m_iInputDirection = 0;
						goto end_of_loop;
					}
					if( !pCon[this_scm->iCallNum+iMaxAllowedCall].bOpendChannel )
					{
						break;
					}
					if( hEvent )
						WaitForSingleObject( hEvent, 100 ); 
				}
				pCon[this_scm->iCallNum].m_iInputDirection = 0;
			}
			else if( step.sub_item.function.iStopUserInput >= 13 )
			{
				while( pCon[this_scm->iCallNum].bOpendChannel)
				{
					if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
					{
						scm->iProcState = 2;
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						if( hEvent )
						{
							CloseHandle( hEvent );
							hEvent = NULL;
						}
						this_scm->m_bCon = FALSE;
						CloseHandle( pCon[this_scm->iCallNum].thread_sce);
						pCon[this_scm->iCallNum].thread_sce = NULL;
						return 1;
					}

					if( hEvent )
						WaitForSingleObject( hEvent, 200 );
				}
			}
			else if( step.sub_item.function.iStopUserInput <= 14 )
			{
				while( pCon[this_scm->iCallNum+iMaxAllowedCall].bOpendChannel)
				{
					if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
					{
						scm->iProcState = 2;
						if( hEvent )
						{
							CloseHandle( hEvent );
							hEvent = NULL;
						}
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						this_scm->m_bCon = FALSE;
						CloseHandle( pCon[this_scm->iCallNum].thread_sce);
						pCon[this_scm->iCallNum].thread_sce = NULL;
						return 1;
					}

					if( hEvent )
						WaitForSingleObject( hEvent, 200 );
				}
			}
			else if( step.sub_item.function.iStopUserInput >= 15 &&
					step.sub_item.function.iStopUserInput <= 360 )
			{
				step.sub_item.function.iStopUserInput -=14;
				int iMaxVal = (step.sub_item.function.iStopUserInput *1000)/200;
				int count=0;
				while( pCon[this_scm->iCallNum].bOpendChannel)
				{
					if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
					{
						scm->iProcState = 2;
						if( hEvent )
						{
							CloseHandle( hEvent );
							hEvent = NULL;
						}
						WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
						this_scm->m_bCon = FALSE;
						CloseHandle( pCon[this_scm->iCallNum].thread_sce);
						pCon[this_scm->iCallNum].thread_sce = NULL;
						return 1;
					}
					if( hEvent )
						WaitForSingleObject( hEvent, 200 );
					if( count ++ >= iMaxVal )
						break;
				}
			}
			if( step.target_id[0] >= 0)
				SetCurStep(this_scm->iCallNum, step.target_id[0]);
			else
				goto end_of_loop;
			break;
		case 5: // waiting, sleeping. without noticing stop key.
			WriteLogL(PROG_NAME, this_scm->iCallNum , "SCM", "Sleeping in");
			while( pCon[this_scm->iCallNum].bOpendChannel )
			{
				if( !pCon[this_scm->iCallNum].bUsedThisCall || pCon[this_scm->iCallNum].bCallReusing)
				{
					scm->iProcState = 2;
					if( hEvent )
					{
						CloseHandle( hEvent );
						hEvent = NULL;
					}
					WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
					this_scm->m_bCon = FALSE;
					CloseHandle( pCon[this_scm->iCallNum].thread_sce);
					pCon[this_scm->iCallNum].thread_sce = NULL;
					return 1;
				}
				if( hEvent )
					WaitForSingleObject( hEvent, 100 ); 
			}
			WriteLogL(PROG_NAME, this_scm->iCallNum , "SCM", "Sleeping out");
			goto end_of_loop;
			break;
		case 6: // MultiPoint Hub
			{
				int selectedDtmf=-1,togoVal=-1;
				item_hub *pHub = &step.sub_item.hub;
				if( pHub->iCompDtmfBufNum<0 || pHub->iCompDtmfBufNum >= 5)
				{
					SetCurStep(this_scm->iCallNum,pHub->falsegoto);
				}
				else
				{
					if( this_scm->szArgIn[pHub->iCompDtmfBufNum][0]>='0' &&
						this_scm->szArgIn[pHub->iCompDtmfBufNum][0]<='9' )
					{
						selectedDtmf = this_scm->szArgIn[pHub->iCompDtmfBufNum][0]-48;
					}
					else if( this_scm->szArgIn[pHub->iCompDtmfBufNum][0] == '*')
					{
						selectedDtmf = 10;
					}
					else if( this_scm->szArgIn[pHub->iCompDtmfBufNum][0] == '#')
					{
						selectedDtmf = 11;
					}
					else
						selectedDtmf = -1;
					if( selectedDtmf >= 0 )
					{
						for( int i = 0; i<pHub->iDtmfCount ; i++)
						{
							if( pHub->iDtmf[i] == selectedDtmf )
							{
								togoVal = pHub->iTogo[i];
								break;
							}
						}
						if( togoVal >= 0 )
						{
							SetCurStep(this_scm->iCallNum,togoVal);
						}
						else
						{
							SetCurStep(this_scm->iCallNum,pHub->falsegoto);
						}
					}
					else
					{
						SetCurStep(this_scm->iCallNum,pHub->falsegoto);
						if( pHub->falsegoto == -1 )
						{
							PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
							goto end_of_loop;
						}
					}
				}
			}
			break;
		case 7:// scenario page move
			{
				/* 지정된 다른 시나리오로 이동 */
				int iPageCount = getCount_scenario_fname(step.sub_item.function.dll_name);
				if( iPageCount != -1 )
				{
					this_scm->iCurPage = iPageCount;
					this_scm->iCurStep = 0;
					SetCurStep(this_scm->iCallNum,0);
				}
				else
				{
					this_scm->iCurStep = -1;
					PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum, (long)cmReasonTypeUndefinedReason);
					goto end_of_loop;
				}
			}
			break;
		case 10: // time_chk
			{
				int bOk=0;
				WriteLogL(PROG_NAME, this_scm->iCallNum , "SCM", "TimeChk");
				if( !pCon[this_scm->iCallNum].bOpendChannel )
					goto end_of_loop;
				if( step.sub_item.time.bSepecialDayChk)
				{
					if( SpecialDayChk(&step.sub_item.time.sfname[0]) )
					{
						bOk = 1;
						SetCurStep(this_scm->iCallNum, step.sub_item.time.iChk_go);
					}
				}
				if( step.sub_item.time.bDailyChk && !bOk)
				{
					//if( DailyChk(&step.sub_item.time) )
					if( DailyChk(&step.sub_item.time.dfname[0]) )
					{
						bOk = 1;
						SetCurStep(this_scm->iCallNum, step.sub_item.time.iDailyChk_go);
					}
				}
				if( !bOk)
				{
					SetCurStep(this_scm->iCallNum, step.target_id[0]);
				}
			}
			break;
		default:
			{
				scm->iProcState = 2;
				if( hEvent )
				{
					CloseHandle( hEvent );
					hEvent = NULL;
				}
				WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point1");
				this_scm->m_bCon = FALSE;
				CloseHandle( pCon[this_scm->iCallNum].thread_sce);
				pCon[this_scm->iCallNum].thread_sce = NULL;
				return 1;
			}
			break;
		}//switch( step.type)
		if( step.target_count == -1)
		{
			break;
		}
	}
end_of_loop: // 한 시나리오의 종료.
	scm->iProcState = 2;
	WriteLogL(PROG_NAME, this_scm->iCallNum, "SCM", "Scenario End - point0");

	if( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}
	this_scm->m_bCon = FALSE;
	CloseHandle( pCon[this_scm->iCallNum].thread_sce);
	pCon[this_scm->iCallNum].thread_sce = NULL;
	return 0;
}

void remove_sharp_star(char * szBuffer)
{
	int iLen = strlen(szBuffer);
	for( int i = 0; i < iLen; i++)
	{
		if( szBuffer[i] == '#' || szBuffer[i] == '*')
		{
			for( int b = i; b < iLen-1; b++)
			{
				szBuffer[b] = szBuffer[b+1];
			}
			szBuffer[iLen-1] = 0;
		}
	}
}

// 사용자의 입력을 받아서 입력 버퍼 큐에 쌓음.
void UserInput(int iCallNum, char *szIndication, int iLength)
{
	SCM * this_scm = NULL;
	//int iScmNumber;
	if( iCallNum >= iMaxAllowedCall && iCallNum < iMaxAllowedCall*2)
		this_scm = &scm[iCallNum-iMaxAllowedCall];
	else if(iCallNum >= 0 && iCallNum < iMaxAllowedCall)
		this_scm = &scm[iCallNum];
	else
		this_scm = NULL;
	
	if( this_scm)
	{
		if( !this_scm->bUserInputIgnore )
		{
			
//
//			if( !this_scm->bInputDirect && iCallNum<iMaxAllowedCall )
//			{
				this_scm->bUserPressedKey = FALSE;
				this_scm->iStepState = 2;
				strcat(this_scm->szInput , szIndication);
//			}
//			else if( this_scm->bInputDirect && iCallNum>=iMaxAllowedCall )
//			{
//				this_scm->bUserPressedKey = FALSE;
//				this_scm->iStepState = 2;
//				strcat(this_scm->szInput , szIndication);
//			}
		}
	}
}

//typedef int (VirtualFunc)(	CTypedPtrList<CObList,StringBuf*> &pList,MENT_BUF & ment_buf, int &iNumber,int iLanguage);
//if return value is -1 : call drop
//if return value is 0 : step hold -- goto step ment
//if return value is 2 : step hold -- goto step ment
//if return value is 3 : step hold -- ready user input
//if return value is 1 : step next go
int FunctionManager(int iCallNum, SStack * pStack,SCENARIO_STEP * step, int &CurStep)
{
	SCM				*this_scm = &scm[iCallNum];
	item_function	*Function = &step->sub_item.function;

	int				nRetry = Function->retrial;
	int				sub_type = Function->sub_type;
	char			LogBuff[256];
	int				iLen = 0;
	
	for( int iT = 0; iT < 5; iT++)
	{
		iLen = strlen( Function->szArgIn[iT]);
		if( iLen > 0 && iLen <= 48)
		{
			memset( this_scm->szArgIn[iT], 0, 48);
			memcpy( this_scm->szArgIn[iT], Function->szArgIn[iT], iLen);
			//pDlg->WriteToConsole(iT, "*****", szArgIn[iT]);
		}
	}
	if( sub_type <= 100)
	{
		if( sub_type < 10)
		{
			switch(sub_type)
			{
			case 0: // go to
				if( step->target_id[0] >= 0)
					SetCurStep(iCallNum, step->target_id[0]);
				else
					step->target_count = -1;
				return 4;
			case 1: // language select
				{
					//int iT = atoi(this_scm->szArgIn[Function->iArgIn[0]-5]);
					int iT = atoi(this_scm->szArgIn[0]);
					if( 0< iT && iT <= iInstalledLang )
					{
						this_scm->iLanguage = iT-1;
						//this_scm->iRetrial = 0;
						this_scm->iRetrialVal = 0;
						SetCurStep(iCallNum, step->target_id[0]);
						//iInputStep ++;
					}
					else
					{
						if( ++this_scm->iRetrialVal >= nRetry)
						//if( ++this_scm->iRetrial >= nRetry)
						{
							if( Function->falsegoto == -1 )
								return -1;
							else
							{
								SetCurStep(iCallNum, Function->falsegoto);
								this_scm->iRetrialVal = 0;
								//this_scm->iRetrial = 0;
								return 4;
							}
						}
						return 0;
					}
				}
				break;
			case 3: // drop call
				return -1;
//			case 4: // goto other scenario
//				{
//					this_scm->iCurPage = GetStack(iCallNum, Function->dll_name);
//					if( this_scm->iCurPage == -1)
//						return -1;
//					memset( this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
//					//iInputStep = 0;
//				}
//				break;
			case 5: // call forward
				PostMessage(m_hWnd, WM_VOICON_CallForward, this_scm->m_pCon->iCallNumber, 0);
				if( step->target_id[0] >= 0)
					SetCurStep(iCallNum, step->target_id[0]);
				else
					step->target_count = -1;
				return 4;
			case 8: // call forward, not h450
				this_scm->m_pCon->bInChannelConnect = 1;
				PostMessage( m_hWnd, WM_VOICON_CallDrop, this_scm->iCallNum+iMaxAllowedCall, (long)cmReasonTypeUndefinedReason);
				while( pCon[this_scm->iCallNum+iMaxAllowedCall].bUsedThisCall)
				{
					Sleep(18);
				}
				SendMessage(m_hWnd, WM_VOICON_CallMake, this_scm->m_pCon->iCallNumber, 0);
				if( step->target_id[0] >= 0)
					SetCurStep(iCallNum, step->target_id[0]);
				else
					step->target_count = -1;
				return 4;
			} // switch(sub_type)
		}
		else if( sub_type >= 10 && sub_type < 20 )
		{
			int iRetVal = atoi(this_scm->szArgIn[Function->iArgIn[0]-5]);
			int iInBuffer = 0;
			for( int i = 1; i< Function->iArgInCount; i++)
			{
				iInBuffer = Function->iArgIn[i];
				switch(sub_type%10)
				{
				case 0: // mathematical function +
					iRetVal += atoi(this_scm->szArgIn[iInBuffer-5]);
					break;
				case 1: // mathematical function -
					iRetVal -= atoi(this_scm->szArgIn[iInBuffer-5]);
					break;
				case 2: // mathematical function *
					iRetVal *= atoi(this_scm->szArgIn[iInBuffer-5]);
					break;
				case 3: // mathematical function /
					iRetVal /= atoi(this_scm->szArgIn[iInBuffer-5]);
					break;
				case 4: // logical function &
					iRetVal &= atoi(this_scm->szArgIn[iInBuffer-5]);
					break;
				case 5: // logical function |
					iRetVal |= atoi(this_scm->szArgIn[iInBuffer-5]);
					break;
				}//switch(sub_type%10)
				if( Function->iArgOutCount == 1)
				{
					if( Function->iArgOut[0] >= 5 && Function->iArgOut[0]< 10)
						wsprintf(this_scm->szArgIn[Function->iArgOut[0]-5], "%d", iRetVal);
				}
				if( step->target_count == 1)
				{
					SetCurStep(iCallNum, step->target_id[0]);
					return 4;
				}
				else
				{
					step->target_count = -1;
				}
			} // for( int i = 1; i< Function->iArgInCount; i++)
		}
		else if(sub_type >= 20 && sub_type < 30 )
		{
			int bRet = 0;
			int iInBuffer0 = Function->iArgIn[0]-5;
			int iInBuffer1 = Function->iArgIn[1]-5;
			
			//pDlg->WriteToConsole(iInBuffer0, "20", Function->szArgIn[iInBuffer0]);
			//pDlg->WriteToConsole(iInBuffer1, "20", Function->szArgIn[iInBuffer1]);
			
			if( strlen( Function->szArgIn[iInBuffer0])> 0 && strlen( Function->szArgIn[iInBuffer0]) < 48)
			{
				wsprintf(this_scm->szArgIn[iInBuffer0], "%s", Function->szArgIn[iInBuffer0]);
			}
			if( strlen( Function->szArgIn[iInBuffer1])> 0 && strlen( Function->szArgIn[iInBuffer1]) < 48)
			{
				wsprintf(this_scm->szArgIn[iInBuffer1], "%s", Function->szArgIn[iInBuffer1]);
			}
			switch(sub_type%20)
			{
			case 0: // if equal or not equal: if(a==b) or if(a!=b)
				//pDlg->WriteToConsole(iInBuffer0, "20", szArgIn[iInBuffer0]);
				//pDlg->WriteToConsole(iInBuffer1, "20", szArgIn[iInBuffer1]);
				//if( memcmp(this_scm->szArgIn[iInBuffer0], 
				//	this_scm->szArgIn[iInBuffer1],strlen( this_scm->szArgIn[iInBuffer0]))==0)
				{
#if 0
					if( atoi( this_scm->szArgIn[iInBuffer0]) == atoi( this_scm->szArgIn[iInBuffer1]))
					{
						bRet = 1;
					}
#else
					char tbuf1[64];
					char tbuf2[64];
					// left item
					if( Function->iArgIn[0] >= 5 )
					{
						sprintf(tbuf1,"%s", this_scm->szArgIn[Function->iArgIn[0]-5]);
					}
					else if( Function->iArgIn[0] == 0 )
					{
						sprintf(tbuf1, "%s", pCon[this_scm->iCallNum].des_e164 );
					}
					else if( Function->iArgIn[0] == 1 )
					{
						sprintf(tbuf1, "%s", pCon[this_scm->iCallNum].src_e164 );
					}
					else if( Function->iArgIn[0] == 2 )
					{
						sprintf(tbuf1, "%s", pCon[this_scm->iCallNum].cID );
					}
					// right item
					if( Function->iArgIn[1] >= 5 )
					{
						sprintf(tbuf2,"%s", this_scm->szArgIn[Function->iArgIn[1]-5]);;
					}
					else if( Function->iArgIn[1] == 0 )
					{
						sprintf(tbuf2, "%s", pCon[this_scm->iCallNum].des_e164 );
						//sprintf(tbuf1,"%s", this_scm->
					}
					else if( Function->iArgIn[1] == 1 )
					{
						sprintf(tbuf2, "%s", pCon[this_scm->iCallNum].src_e164 );
					}
					else if( Function->iArgIn[1] == 2 )
					{
						sprintf(tbuf2, "%s", pCon[this_scm->iCallNum].cID );
					}

					if( !strcmp(tbuf1, tbuf2) )
					{
						bRet = 1;
					}
#endif
				}
				break;
			case 2: // if upper or below : if(a>b) or if(a<b), Number Only
				if( atoi(this_scm->szArgIn[iInBuffer0]) > atoi(this_scm->szArgIn[iInBuffer1]) )
					bRet = 1;
				break;
			case 4: // || : if(a||b), number only
				if( atoi(this_scm->szArgIn[iInBuffer0]) || atoi(this_scm->szArgIn[iInBuffer1]) )
					bRet = 1;
				break;
			case 5: // && : if(a&&b), number only
				if( atoi(this_scm->szArgIn[iInBuffer0]) && atoi(this_scm->szArgIn[iInBuffer1]) )
					bRet = 1;
				break;
			} // switch(sub_type%20)
			if( Function->iArgOutCount == 1)
			{
				if( Function->iArgOut[0] >= 5 &&  Function->iArgOut[0] <10)
				{
					//pDlg->WriteToConsole(iInBuffer1, "20", szArgIn[iInBuffer1]);
					wsprintf(this_scm->szArgIn[Function->iArgOut[0]-5], "%d", bRet);
					//pDlg->WriteToConsole(Function->iArgOut[0], "OutVal", szArgIn[Function->iArgOut[0]]);
				}
			}
			if( bRet == 0)
			{
				// 틀린 경우
				SetCurStep(iCallNum, Function->falsegoto);
				return 4;
			}
			else
			{
				// 맞는 경우.
				if( step->target_count == 1)
				{
					SetCurStep(iCallNum, step->target_id[0]);
					return 4;
				}
				else
					step->target_count = -1;
			}
		}
		else if(sub_type >= 50 && sub_type < 60 )
		{
			
			//int iInBuffer0 = Function->iArgIn[0]-5;
			int iInBuffer0 = Function->iArgIn[0];
			switch(sub_type )
			{
			case 50:
				if( iInBuffer0 < 0 )
					iInBuffer0 =0;
				// modified 2006/09/07 by swhors@naver.com
				// 기능 추가 중 변경 된 버퍼의 지정 번호 수정.
				//sprintf(this_scm->szArgIn[iInBuffer0], "%s", Function->szArgIn[iInBuffer0 ]);
				// ->
				if( Function->iArgOutCount > 0 )
				{
					if( Function->iArgOut[0] > 4 && Function->iArgOut[0] <10 )
					{
						if( iInBuffer0 > 4 )
						{
							sprintf(this_scm->szArgIn[iInBuffer0-5], "%s",
								this_scm->szArgIn[Function->iArgOut[0]-5] );
						}
						else
						{
							memset(this_scm->billingPacket.pBody.Destination, 0x20, 20);
							sprintf(this_scm->billingPacket.pBody.Destination, 
								"%s", this_scm->szArgIn[Function->iArgOut[0]-5]);
						}
					}
				}
				else
				{
					if( iInBuffer0 > 4 )
					{
						sprintf(this_scm->szArgIn[iInBuffer0-5], "%s", Function->szArgIn[iInBuffer0-5]);
					}
					else
					{
						memset(this_scm->billingPacket.pBody.Destination, 0x20, 20);
						sprintf(this_scm->billingPacket.pBody.Destination, 
							"%s", Function->szArgIn[iInBuffer0-5]);
					}
				}

				SetCurStep(iCallNum, step->target_id[0]);
				return 4;
			case 51: // dec
				this_scm->iRetrialCurrentVal --;
				this_scm->iRetrialVal --;
				if( this_scm->iRetrialVal <= 0 )
				{
					SetCurStep(iCallNum, Function->falsegoto);
					if( this_scm->bResetValEnable )
					{
						if( this_scm->m_hMod )
						{
							ResetValue * reset = (ResetValue *)::GetProcAddress(this_scm->m_hMod, "ResetValue");
							if( reset)
								reset(iCallNum);
						}
					}
				}
				else
					SetCurStep(iCallNum, step->target_id[0]);
				return 4;
			case 52: // inc
				this_scm->iRetrialCurrentVal ++;
				if( this_scm->iRetrialVal <= this_scm->iRetrialCurrentVal )
				{
					SetCurStep(iCallNum, Function->falsegoto);
					if( this_scm->bResetValEnable )
					{
						if( this_scm->m_hMod )
						{
							ResetValue * reset = (ResetValue *)::GetProcAddress(this_scm->m_hMod, "ResetValue");
							if( reset)
								reset(iCallNum);
						}
					}
				}
				else
					SetCurStep(iCallNum, step->target_id[0]);
				return 4;
			case 53: // set
				this_scm->iRetrialVal = atoi(step->sub_item.function.szArgIn[0]);
				this_scm->iRetrialCurrentVal = 0;
				SetCurStep(iCallNum, step->target_id[0]);
				return 4;
			}
		}
		
	}
	else if( sub_type >= 101 )// for external function dll
	{
		this_scm->m_hMod = FindPlugHandle(Function->dll_name);
		int iRet = 0;
		
		if( !this_scm->m_hMod )
		{
			return -1;
		}
		if( sub_type == 101 )
		{ // General
			
			VirtualFunc *mod = (VirtualFunc *)::GetProcAddress(this_scm->m_hMod,Function->function_name);
			
			if( !mod )
			{
				return -1;
			} //
			
			int i = 0;
			char sBuffer[20][128];
			
			for( i = 0; i< 20; i++)
			{
				memset( sBuffer[i], 0, 128);
			}
			
			if( Function->iArgInCount > 10 )
				Function->iArgInCount = 10;
			for( i = 0 ; i < Function->iArgInCount; i++)
			{
				if( Function->iArgIn[i] == 2)
				{
					// Calling Party CID
					int iLen = 17;
					if( !this_scm->m_pCon->bUsedThisCall )
						return -1;
					if( this_scm->m_pCon->hsCall )
					{
						memcpy( sBuffer[i], this_scm->m_pCon->cID, 16);
					}
				}
				else if( Function->iArgIn[i] == 4)
				{ // Called Party CID
					if( bEnableCallCoupl)
					{
						int iOther = this_scm->m_pCon->iCallNumber+iMaxAllowedCall;
						int iLen = 128;
						if( pCon[iOther].hsCall && pCon[iOther].bUsedThisCall)
						{
							memcpy( sBuffer[i], pCon[iOther].cID, 16);
						}
					}
				}
				else if( Function->iArgIn[i] == 0)
				{ // Calling Party E164
					memcpy(sBuffer[i], this_scm->m_pCon->src_e164, strlen(this_scm->m_pCon->src_e164));
				}
				else if( Function->iArgIn[i] == 1)
				{ // Called Party E164
#if 0
					if( bEnableCallCoupl)
					{
						int iOther = this_scm->m_pCon->iCallNumber+iMaxAllowedCall;
						int iLen = 128;
						if( strlen(this_scm->m_pCon->des_e164) > 0 )
							memcpy(sBuffer[i], this_scm->m_pCon->des_e164, strlen(this_scm->m_pCon->des_e164));
					}
#else
					sprintf(sBuffer[i], "%s", this_scm->billingPacket.pBody.Destination);
#endif
				}
				else if( Function->iArgIn[i] == 3)
				{ // GKIP
					int iLLen = strlen(szGlobalGkIp);
					if( iLLen < 128 )
						memcpy((char*)&sBuffer[i][0], szGlobalGkIp, iLLen);
				}
				else if( Function->iArgIn[i] == 10)
				{ // 사용자선택 언어.
					wsprintf((char*)&sBuffer[i][0], "%d", this_scm->iLanguage);
				}
				else if( Function->iArgIn[i] == 11)
				{ // 사용자 입력 버퍼
					// 사용되지 않음.
					memcpy(sBuffer[i], this_scm->szInput, 16);
				}
				else 
				{ // 기타 사용자 버퍼.
					wsprintf(sBuffer[i], "%s", this_scm->szArgIn[Function->iArgIn[i]-5]);
				}
			}
			//pDlg->WriteToConsole(iRet, "101 ******", sBuffer[0]);
			//pDlg->WriteToConsole(iRet, "101 ******", sBuffer[1]);
			int iInOutVal = Function->iArgInCount;
			int iMentCount = iCallNum;
			
			iRet = mod( &this_scm->billingPacket, &iInOutVal, sBuffer, &iMentCount, &nRetry, &this_scm->m_pCon->iCallState);
			//pDlg->WriteToConsole(iRet, "101 ******", "");
			//pDlg->WriteToConsole(iMentCount, "101 ******", "");
						
			if( Function->iArgOut[0] >= 0 && Function->iArgOut[0] < 10)
			{
				if( Function->iArgOut[0] >=5 && Function->iArgOut[0] < 10 )
					wsprintf(this_scm->szArgIn[Function->iArgOut[0]-5], "%d", iInOutVal);
				else if( Function->iArgOut[0] ==1 )
				{
					memset(this_scm->billingPacket.pBody.Destination, 0x20, 20);
					wsprintf(this_scm->billingPacket.pBody.Destination, "%s", sBuffer[0]);
				}
			}
			
			if( iRet == -1)
			{
				//실패인 경우 입력 버퍼를 초기화 함.
				this_scm->bUserInputIgnore = FALSE;
				memset(this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
				this_scm->iReadCount = 0;
			}
			else
			{
				this_scm->m_hMod = NULL;
			}
			if( iMentCount > 0)
			{
				for( i = 0; i < iMentCount ; i++)
				{
					if( this_scm->iStepState == 2 && iRet != -1)
						break;
					
					if( iRet == -1 && this_scm->iStepState == 2)
					{
						this_scm->iStepState = 0;
					}
					if( !this_scm->m_pCon->bUsedThisCall)
						return -1;
					if( RtpPlay(this_scm->m_pCon->iChannelCodec,*this_scm->m_pCon, sBuffer[i], sLanguage[this_scm->iLanguage], this_scm->iStepState, iRet, this_scm->bUserInputIgnore ) == -1)
					{
						return -1;
					}
					//WriteLog("101", 0, "Function 101", sBuffer[i]);
					//pDlg->WriteToConsole(iCallNum, "Function RtpPlay", sBuffer[i]);
					Sleep(60);
				}
			}
		}
		else if( sub_type == 102)// Support Dongseo IT Billing System
		{ 
			// 이 함수는 동서정보기술의 빌링 시스템과 호완을 위한 것으로
			// 두 개의 파라미터만이 사용된다.
			// 인증 서버의 IP와 포트 번호이다.
			// 인증 서버의 IP와 포트 번호로 인증서버에서 잔액 확인이나, 사용여부등의
			// 정보를 가져오기 위해서 사용된다.
			
			DongSeoBilling *mod = (DongSeoBilling *)::GetProcAddress(this_scm->m_hMod,Function->function_name);
			if( !mod )
			{
				return -1;
			} //
			char sBuffer[20][128];
			int iMentCount = this_scm->iCallNum;
			int i = 0;
			
			for( i = 0; i< 20; i++)
			{
				memset( sBuffer[i], 0, 128);
			}
			for( i = 0 ; i < Function->iArgInCount; i++)
			{
				if( Function->iArgIn[i] == 0)
				{ // Calling Party E164
					int iLLen = strlen(this_scm->m_pCon->src_e164);
					if( iLLen < 128)
						memcpy(sBuffer[i], this_scm->m_pCon->src_e164, iLLen);
				}
				else if( Function->iArgIn[i] == 1)
				{ // Called Party E164
					int iLLen = strlen(this_scm->m_pCon->des_e164);
					if( iLLen < 128)
						memcpy(sBuffer[i], this_scm->m_pCon->des_e164, iLLen);
				}
				else if( Function->iArgIn[i] == 3)
				{ // GKIP
					int iLLen = strlen(szGlobalGkIp);
					if( iLLen < 128)
						memcpy(sBuffer[i], szGlobalGkIp, iLLen);
				}
				else if( Function->iArgIn[i] == 10)
				{ // 사용자선택 언어.
					wsprintf(sBuffer[i], "%d", this_scm->iLanguage);
				}
				else if( Function->iArgIn[i] == 11)
				{ // 사용자 입력 버퍼
					// 사용되지 않음.
					memcpy(sBuffer[i], this_scm->szInput, 16);
				}
				else 
				{ // 기타 사용자 버퍼.
					wsprintf(sBuffer[i], "%s", this_scm->szArgIn[Function->iArgIn[i]-5]);
				}
			}
			
			iRet = mod( &this_scm->billingPacket, sBuffer, &iMentCount, &nRetry);
			
			if( Function->iArgOut[0] >= 5 && Function->iArgOut[0] < 10)
			{
				wsprintf(this_scm->szArgIn[Function->iArgOut[0]-5], "%d", iRet);
			}
			
			if( iRet == -1)
			{
				//실패인 경우 입력 버퍼를 초기화 함.
				memset(this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
				this_scm->iReadCount = 0;
			}
			
			if( iRet > 0 ) iRet = 0;
			if( !iRet )
				this_scm->m_hMod = NULL;

			if( iMentCount > 0)
			{
				for( i = 0; i < iMentCount ; i++)
				{
					
					if( this_scm->iStepState == 2 && iRet != -1)
						break;
					if( iRet == -1 && this_scm->iStepState == 2)
					{
						this_scm->iStepState = 0;
					}
					if( !this_scm->m_pCon->bUsedThisCall)
						return -1;
					if( RtpPlay(this_scm->m_pCon->iChannelCodec,*this_scm->m_pCon, sBuffer[i], sLanguage[this_scm->iLanguage], this_scm->iStepState, iRet, this_scm->bUserInputIgnore) == -1)
					{
						//pDlg->WriteToConsole(iCallNum, "Function RtpPlay", sBuffer[i]);
						return -1;
					}
					Sleep(40);
				}
			}
		}
		else if( sub_type == 103) // Support DongSeo IT Gatekeeper
		{
			
			// 동서정보기술의 게이트키퍼와 정보 공유를 위해서 사용된다.
			// 파라미터는 ??
			DongSeoGK *mod = (DongSeoGK *)::GetProcAddress(this_scm->m_hMod,Function->function_name);
			
			if( !mod )
			{
				return -1;
			} //
			
			char sBuffer[20][128];
			int iMentCount = this_scm->iCallNum;
			int i = 0;
			
			for( i = 0; i< 20; i++)
			{
				memset( sBuffer[i], 0, 128);
			}
			if( !this_scm->m_pCon->bUsedThisCall)
				return -1;
			iRet = mod(	&this_scm->billingPacket, sBuffer, &iMentCount, &nRetry, &this_scm->iLanguage, &this_scm->m_pCon->iCallState);
			
			if( iRet == -1)
			{
				//실패인 경우 입력 버퍼를 초기화 함.
				memset(this_scm->szInput, 0, MAX_DTMF_INPUT_LENGTH);
				this_scm->iReadCount = 0;
			}
			else
			{
				this_scm->m_hMod = NULL;
			}
			
			if( iMentCount > 0)
			{
				for( i = 0; i < iMentCount ; i++)
				{
					if( this_scm->iStepState == 2 && iRet != -1)
						break;
					
					if( iRet == -1 && this_scm->iStepState == 2)
					{
						this_scm->iStepState = 0;
					}
					if( !this_scm->m_pCon->bUsedThisCall)
						return -1;
					if( RtpPlay(this_scm->m_pCon->iChannelCodec,*this_scm->m_pCon, sBuffer[i], sLanguage[this_scm->iLanguage], this_scm->iStepState, iRet, this_scm->bUserInputIgnore) == -1)
					{
						//pDlg->WriteToConsole(iCallNum, "Function RtpPlay", sBuffer[i]);
						return -1;
					}
					Sleep(60);
				}
			}
		}
		
		memset( LogBuff, 0 ,256 );
		wsprintf(LogBuff, "retri:%d iretri:%d",nRetry, this_scm->iRetrialVal );
		//wsprintf(LogBuff, "retri:%d iretri:%d",nRetry, this_scm->iRetrial );
//		WriteLog2(PROG_NAME, iCallNum,"SCM", LogBuff);

		if( iRet == -1)
		{
			this_scm->bResetValEnable = 1;
			//if( ++this_scm->iRetrialVal >= this_scm->iRetrialCurrentVal )
			//if( ++this_scm->iRetrial >= nRetry )
			//if( this_scm->iRetrialVal <= this_scm->iRetrialCurrentVal )
			//{
			//	ResetValue * reset = (ResetValue *)::GetProcAddress(hMod, "ResetValue");
				
			//	if( reset)
			//		reset(iCallNum);
//		wsprintf(LogBuff, "iRet:%d  nRet:%d",this_scm->iRetrial, nRetry );
//		WriteLogL(PROG_NAME, iCallNum,"SCM", LogBuff);
				if( Function->falsegoto != -1)
				{
					SetCurStep(iCallNum, Function->falsegoto);
					//this_scm->iRetrial = 0;
					//this_scm->iRetrialVal = 0;
				}
				else 
				{
					return -1;
				}
				return 4;
			//}
			//else 
			//{
			//	return 0;
			//}
		}
		else if( iRet == 0 )
		{
			this_scm->bResetValEnable = 0;
			if( step->target_count > 0)
			{
				memset( LogBuff, 0 ,256 );
//				wsprintf(LogBuff, "tcount:%d tid:%d",step->target_count, step->target_id[0] );
//				WriteLogL(PROG_NAME, iCallNum,"SCM", LogBuff);
				if( step->target_id[0] >= 0)
				{
					SetCurStep(iCallNum, step->target_id[0]);
					//this_scm->iRetrial = 0;
					//this_scm->iRetrialVal = 0;
					return 4;
				}
				else
				{
					return 5;
				}
			}
			else
				return 5;
		}
	}
	return 1;
}

HMODULE FindPlugHandle(char *szDllName)
{
	return GetPlugInHandle(szDllName);
}

void SetCurStep(int iCallNum, int iID)
{
	SCM * this_scm = &scm[iCallNum];
#if 0
	int iCount = GetScenarioCount(this_scm->iCurPage);
	if( iCount < 0 )
		return;
	if( iID == -1)
	{
		this_scm->iCurStep = iCount;
		return;
	}
	else
#endif
		this_scm->iCurStep = iID;
}