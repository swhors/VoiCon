#pragma warning(disable:4786)
#include "stdafx.h"
#include <vector>
#include <algorithm>

#include "operate_scenario.h"

#include "typedef.h"

#include "voice.h"
#include "ivrkeydata.h"
#include "SystemBaseSet.h"

#include "operate_ment.h"

extern SStack *mpStack;

std::vector<std::string> scenario_file;

void insert_scenario_fname(char *fname);
void delete_scenario_fname(char *fname);
int	size_scenario_fname();
int getCount_scenario_fname(char *fName);


void insert_scenario_fname(char *fname)
{
	std::vector<std::string>::iterator b,e,f;
	b = scenario_file.begin();
	e = scenario_file.end();
	if( b == e )
	{
		scenario_file.push_back(fname);
		sf_count ++;
	}
	else
	{
		f = std::find(b,e,fname );
		if( f == e )
		{
			sf_count ++;
			scenario_file.push_back(fname);
		}
	}
}

void delete_scenario_fname(char *fname)
{
	std::vector<std::string>::iterator b,e,f;
	b = scenario_file.begin();
	e = scenario_file.end();
	if( b == e)
		return;
	f = std::find(b, e, fname );
	if( f != e )
	{
		sf_count --;
		scenario_file.erase(f);
	}
}

int	size_scenario_fname()
{
	return scenario_file.size();
}

int getCount_scenario_fname(char *fName)
{
	int i= 0;
	std::vector<std::string>::iterator b,e,f;
	b = scenario_file.begin();
	e = scenario_file.end();
	f = std::find(b,e,fName);
	if( f != e )
	{
		return (int)(f-b);
	}
	return -1;
}

BOOL FindScenarioFile()
{
	FILE *fp;
	TCHAR	tBuf[64];
	sf_count = 0;
	fp = fopen("scenario.ini", "r");
	if( fp )
	{
		while(fgets(tBuf, 64, fp))
		{
			int len = strlen(tBuf );
			if( len > 0 )
			{
				for(int i = 0; i< len ; i++)
				{
					if( tBuf[i] == '\n' ||
						tBuf[i] == '\r' )
						tBuf[i] = 0;
				}
				insert_scenario_fname(tBuf);
			}
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

int	GetScenarioCount(int iPage)
{
    if( sf_count > iPage &&  iPage >= 0)
	{
		return mpStack[iPage].iItemCount;
	}
	return -1;
}

SStack * GetStackPoint(int iPage)
{
    if( sf_count > iPage &&  iPage >= 0)
	{
		return &mpStack[iPage];
	}
	return NULL;
}


int	GetScenarioItem(int iPage, int iNum,SCENARIO_STEP &s)
{
	int iSize = size_scenario_fname();
    if( sf_count > iPage &&  iPage >= 0)
	{
		return mpStack[iPage].getData(iNum, s);
	}
	return 1;
}


BOOL bLoadScenarioFile()
{
	int				nFileLength = 0;
	FILE			*fp;
	SCENARIO_STEP	stepdATA;
	char			szLineBuf[256];
	char			*inbuf=NULL;
	BOOL			bScenarioClosed = TRUE;
	item_uinput		*input;
	item_function	*func;
	item_ment		*ment;
	item_time_chk	*time;
	int				iBeginCount=0;

	std::vector<std::string>::iterator b,e,f;

	
	mpStack = new SStack[sf_count];
	
	int	iSc = 0; // int Scenario Count
	b = scenario_file.begin();
	e = scenario_file.end();
	for( f = b ,iSc = 0; f < e; f++,iSc++)
	{
		char *fName;
		int iLocalCount = 0;
		fName = (char*)f->c_str();
		if( fName)
		{
			//CString csBuf;
			char sFile[MAX_PATH];
			char fFolder[MAX_PATH];
			::GetCurrentDirectory( MAX_PATH,fFolder);
			int ssize = strlen(fName);
			for(int i = 0 ; i< ssize ; i++)
			{
				if( *(fName+i)=='\r'||
					*(fName+i)=='\n')
				{
					*(fName+i) = 0;
				}
			}
			sprintf(sFile, "%s\\scenario\\%s.scm", fFolder,fName);
			if( ( fp = fopen(sFile, "r+t") ) )
			{
				mpStack[iSc].s_page = iSc;
				sprintf(mpStack[iSc].szFile,"%s", (char*)f->c_str());
				inbuf = fgets( szLineBuf, 256, fp);
				mpStack[iSc].iItemCount = 0;
				
				while(inbuf)
				{
					if( !memcmp( szLineBuf, "[BEGIN]", 7) || !memcmp(szLineBuf, "// -- Item Start", 16))
					{
						if( bScenarioClosed == TRUE )
						{
							stepdATA.Init();
							bScenarioClosed = FALSE;
						}
						else
						{
							fclose(fp);
							return FALSE;
						}
					}
					else if ( !memcmp( szLineBuf, "[END]", 5) || !memcmp(szLineBuf, "// -- Item End", 14 ))
					{
						//need refix
						mpStack[iSc].insertData(stepdATA);
						mpStack[iSc].iItemCount++;
						bScenarioClosed = TRUE;
					}
					else if ( memcmp( szLineBuf, "TARGET", 6) == NULL )
					{
						//int iNum = atoi( (char *)&szLineBuf[8]);
						stepdATA.SetTargetID(atoi( (char *)&szLineBuf[8]));
						stepdATA.IncTargetCount();
					}
					else if ( memcmp( szLineBuf, "SELF:", 5) == NULL)
					{
						stepdATA.id = atoi( (char *)&szLineBuf[5]);
					}
					else if ( !memcmp( szLineBuf, "TYPE:", 5) )
					{
						// item's type
						stepdATA.type = atoi((char *)&szLineBuf[5]);
						if( stepdATA.type == 0 || stepdATA.type == IDT_PLAY )
						{
							// menet_play
							stepdATA.sub_item.ment.iMentCount = 0;
							stepdATA.sub_item.ment.iResetUserInput = 0;
							
							for( int i = 0; i < 9; i++)
							{
								memset( stepdATA.sub_item.ment.szMent[i], 0, 180);
							}
							stepdATA.type = 0;
						}
						else if (stepdATA.type == 1 || stepdATA.type == IDT_INPUT )//||stepdATA.type == 3)
						{
							// dtmf input
							stepdATA.sub_item.uinput.init();
							stepdATA.type = 1;
						}
						else if (stepdATA.type == 2 ||
								stepdATA.type == IDT_PLUG ||
								stepdATA.type == IDT_LOGICAL ||
								stepdATA.type == IDT_CONDITION ||
								stepdATA.type == IDT_MATH||
								stepdATA.type == IDT_IS ||
								stepdATA.type == IDT_RETRIAL ||
								stepdATA.type == IDT_INIT ||
								stepdATA.type == IDT_GOTO )//||
								//stepdATA.type == IDT_VOICE_REC||
								//stepdATA.type == IDT_CALLERR)
						{
							// plug, mathematical, conditional, logical
							stepdATA.sub_item.function.init();
							stepdATA.type = 2;
						}
						else if( stepdATA.type == IDT_PAGE )
						{
							stepdATA.sub_item.function.init();
							stepdATA.type = 7;
						}
						else if (stepdATA.type == 3 ||
								stepdATA.type == IDT_MAKE_CALL )
						{
							// make call
							stepdATA.sub_item.call.init();
							stepdATA.type = 3;
						}
						else if (stepdATA.type == 4||
								stepdATA.type == IDT_LOOP)
						{
							int i = 0;
							stepdATA.sub_item.function.iMaxSize = 1024; // 1k byte
							stepdATA.sub_item.function.iStopUserInput = -1; // none;
							
							memset( stepdATA.sub_item.function.dll_name , 0, 128);
							stepdATA.sub_item.function.iDirect=0;
							stepdATA.sub_item.function.sub_type = -1;
							stepdATA.sub_item.function.falsegoto = -1;
							memset( stepdATA.sub_item.function.function_name, 0, 64);
							
							for( i = 0; i< 6; i++)
								memset( stepdATA.sub_item.function.szArgIn[i], 0, 64);
							
							for( i = 0; i< 10; i++)
								stepdATA.sub_item.function.iArgIn[i] = -1;
							
							stepdATA.sub_item.function.iArgInCount = 0;
							
							for( i = 0; i< 9; i++)
								stepdATA.sub_item.function.iArgOut[i] = 20;
							
							stepdATA.sub_item.function.iArgOutCount = 0;
							stepdATA.sub_item.function.retrial = 0;
							stepdATA.type = 4;
						}
						else if( stepdATA.type == 10 || 
								stepdATA.type == IDT_TIMECHK)
						{
							stepdATA.sub_item.time.init();
							stepdATA.type = 10;
						}
						else if( stepdATA.type == IDT_HUB )
						{
							stepdATA.sub_item.hub.init();
							stepdATA.type = 6;
						}
					}
					else
					{
						if( stepdATA.type == 0)
						{
							if ( memcmp( szLineBuf, "MENT", 4) == NULL )
							{
								ment = &stepdATA.sub_item.ment;
								sprintf(ment->szMent[ment->iMentCount], 
									(char *)&szLineBuf[6]);
								mentfileList_push_back((char *)&szLineBuf[6]);
								mentfileList_sort();
								int iLen = strlen( ment->szMent[ment->iMentCount]);
								ment->szMent[ment->iMentCount++][iLen-1] = 0;
							}
							if ( memcmp( szLineBuf, "RESET", 5) == NULL )
							{
								ment = &stepdATA.sub_item.ment;
								ment->iResetUserInput = atoi( (char *)&szLineBuf[7]);
							}
						}
						else if( stepdATA.type == 1 )
						{
							input = &stepdATA.sub_item.uinput;
							if ( memcmp( szLineBuf, "INPUT:", 6) == NULL )
							{
								input->iInput = atoi( (char *)&szLineBuf[6]);
							}
							else if ( memcmp( szLineBuf, "LENGTH:", 7) == NULL )
							{
								input->input_length = atoi( (char *)&szLineBuf[7]);
							}
							else if ( memcmp( szLineBuf, "SHARP:", 6) == NULL )
							{
								input->sharp_key = atoi( (char *)&szLineBuf[6]);
							}
							else if ( memcmp( szLineBuf, "STAR:", 5) == NULL )
							{
								input->star_key = atoi( (char *)&szLineBuf[5]);
							}
							else if ( memcmp( szLineBuf, "SHARPGO:", 8) == NULL )
							{
								input->iSharp_go = atoi( (char *)&szLineBuf[8]);
							}
							else if ( memcmp( szLineBuf, "STARGO:", 7) == NULL )
							{
								input->iStar_go = atoi( (char *)&szLineBuf[7]);
							}
							else if ( memcmp( szLineBuf, "DIRECTION:", 10) == NULL )
							{
								input->iDirect = atoi( (char *)&szLineBuf[10])-1;
							}
							else if ( !memcmp( szLineBuf, "REFRESH:", 8 ) )
							{
								input->iRefresh = atoi( (char *)&szLineBuf[8] );
							}
						}
						else if( stepdATA.type == 3)
						{
							item_call *call = &stepdATA.sub_item.call;
							if( !memcmp( szLineBuf, "TELNUM:", 7) )
							{
								char sbuf[64];
								char *ttbuf;
								wsprintf(sbuf, "%s", (char *)&szLineBuf[7]);
								ttbuf = strchr(sbuf, '\n');
								if( ttbuf )
									*ttbuf = 0;
								ttbuf = strchr(sbuf, '\n');
								if( ttbuf) 
									*ttbuf = 0;
								call->push_calledParty(sbuf);
							}
							else if ( memcmp(szLineBuf, "FALSEGO:", 8) == NULL )
							{
								call->falsegoto = atoi( (char *)&szLineBuf[8]);
							}
							else if ( memcmp( szLineBuf, "ARGIN:", 6) == NULL )
							{
								call->iArgIn[call->iArgInCount++] = 
									atoi( (char *)&szLineBuf[6]);
							}
							else if(!memcmp(szLineBuf,"ENABLEAUTOCALL:", 15))
							{
								call->enableAutoCall = atoi( (char *)&szLineBuf[15]);
							}
							else if(!memcmp(szLineBuf,"ENABLEINCALLNODROP:", 19))
							{
								call->enableInCallNoDrop = atoi((char*)&szLineBuf[19]);
							}
							else if(!memcmp(szLineBuf,"DEFCALLDURATION:", 16))
							{
								call->defCallDuration = atoi((char*)&szLineBuf[16]);
							}
//							else if(!memcmp(szLineBuf,"MAKECALLINOUTFREE:", 18))
//							{
//								call->callinoutFree = atoi((char*)&szLineBuf[18]);
//							}
							else if(!memcmp(szLineBuf,"ENABLEBUSYRETRY:", 16))
							{
								call->enableBusyRetry = atoi((char*)&szLineBuf[16]);
							}
							else if(!memcmp(szLineBuf,"BUSYRETRYCOUNT:", 15))
							{
								call->BusyRetryCount = atoi((char*)&szLineBuf[15]);
							}
							else if(!memcmp(szLineBuf,"BUSYRETRYPOINT:", 15))
							{
								call->BusyRetryPoint = atoi((char*)&szLineBuf[15]);
							}
						}
						else if( stepdATA.type==2 || stepdATA.type == 4)
						{
							/*
								stepdATA.type == IDT_PLUG ||
								stepdATA.type == IDT_LOGICAL ||
								stepdATA.type == IDT_CONDITION ||
								stepdATA.type == IDT_MATH ||
								stepdATA.type == IDT_IS)
							*/
							func = &stepdATA.sub_item.function;
							if( !memcmp( szLineBuf, "ARGIN:", 6) ||
								!memcmp( szLineBuf, "INPUT:", 6) )
							{
								// == or = 에 해당되는 기능의 입력 버퍼.
								func->iArgIn[func->iArgInCount++] = 
									atoi( (char *)&szLineBuf[6]);
							}
							else if( !memcmp(szLineBuf, "SRCBUF:", 7) )
							{
								func->iArgOut[func->iArgOutCount++] =
									atoi((char*)&szLineBuf[7]);
							}
							else if( !memcmp(szLineBuf, "DIRECTION:", 10 ) )
							{
								func->iDirection = atoi((char*)&szLineBuf[10]);
							}
							else if( !memcmp( szLineBuf, "VALUE:", 6 ) )
							{
								if( func->iArgIn[0] >= 5 && func->iArgIn[0] < 10)
								{
									int len;
									// modified 2006/09/07 by swhors@naver.com
									// 기능 추가 중 변경 된 버퍼의 지정 번호 수정.
									sprintf(func->szArgIn[func->iArgIn[0]-5], "%s", &szLineBuf[6]);
									len = strlen(func->szArgIn[func->iArgIn[0]-5]);
									if( func->szArgIn[func->iArgIn[0]-5][len-1] == 10 )
									{
										func->szArgIn[func->iArgIn[0]-5][len-1] = 0;
									}
								}
								else if (func->iArgIn[0] == -1 )
								{
									sprintf(func->szArgIn[0], "%s", &szLineBuf[6]);
								}
								else
								{
									int len;
									sprintf(func->szArgIn[0], "%s", &szLineBuf[6]);
									len = strlen(func->szArgIn[func->iArgIn[0]-5]);
									if( func->szArgIn[func->iArgIn[0]][len-1] == 10 )
									{
										func->szArgIn[func->iArgIn[0]][len-1] = 0;
									}
								}
							}
							else if( !memcmp( szLineBuf, "MTYPE:", 6) )
							{
								switch(atoi( (char *)&szLineBuf[6]))
								{
								case IDMT_PLUS:
									func->sub_type = 10;
									break;
								case IDMT_MINUS:
									func->sub_type = 11;
									break;
								case IDMT_MULTIPLE:
									func->sub_type = 12;
									break;
								case IDMT_DIVIDE:
									func->sub_type = 13;
									break;
								case IDMT_AND:
									func->sub_type = 14;
									break;
								case IDMT_OR:
									func->sub_type = 15;
									break;
								case IDMT_LO_AND:
									func->sub_type = 25;
									break;
								case IDMT_LO_OR:
									func->sub_type = 24;
									break;
								case IDMT_EQUAL:
									func->sub_type = 20;
									break;
								case IDMT_GREATER:
									func->sub_type = 21;
									break;
								case IDMT_LESSER:
									func->sub_type = 22;
									break;
								case IDMT_FILEPLAY:
									func->sub_type = 7;
									break;
								case IDMT_CALLFORWARD1:
									func->sub_type = 8;
									break;
								case IDMT_VOICEREC:
									func->sub_type = 6;
									break;
								case IDMT_DROPCALL:
									func->sub_type = 13;
									break;
								case IDMT_GOTO:
									func->sub_type = 0;
									break;
								case IDMT_GOTOPAGE:
									func->sub_type = 4;
									break;
								case IDMT_LANGUAGESEL:
									func->sub_type = 1;
									break;
								case IDMT_PLUGIN:
									func->sub_type = 101;
									break;
								case IDMT_IS:
								case IDMT_SETVALUE:
									func->sub_type = 50;
									break;
								case IDMT_SETLANGUAGE:
									func->sub_type = 1;
									break;
								//case IDMT_RETRIAL:
								//case IDMT_RETRIAL_IS:
								//	break;
								//case IDMT_RETRIAL_DEC:
								//	break;
								//case IDMT_RETRIAL_INC:
								//	break;
								}
							}
							else if( !memcmp( szLineBuf, "COMMAND:", 8 ) )
							{
								func = &stepdATA.sub_item.function;
								switch( atoi((char*)&szLineBuf[8]) )
								{
								case 0:
									func->sub_type = 51;
									break;
								case 1:
									func->sub_type = 52;
									break;
								default:
									func->sub_type = 53;
									break;
								}
							}
							else if( !memcmp( szLineBuf, "ARGOUT:", 7) )
							{
								func = &stepdATA.sub_item.function;
								func->iArgOut[func->iArgOutCount++] = 
									atoi( (char *)&szLineBuf[7]);
							} 
							else if( !memcmp( szLineBuf, "RETRIAL:", 8) )
							{
								func->retrial = atoi( (char *)&szLineBuf[8]);
							}
							else if( !memcmp( szLineBuf, "STYPE:", 6) )
							{
								func->sub_type = atoi( (char *)&szLineBuf[6]);
							}
							else if( !memcmp( szLineBuf, "ARGINVAL", 8) )
							{
								int ic = szLineBuf[8]-48;
								if( ic >= 5 && ic < 10)
								{
									memset(func->szArgIn[ic-5], 0, 48);
									sprintf(func->szArgIn[ic-5], (char *)&szLineBuf[10]);
									func->szArgIn[ic-5][strlen(func->szArgIn[ic-5]) -1 ] = 0;
								}
								else if (ic == 1)
								{
									memset(func->szArgIn[5], 0, 48);
									sprintf(func->szArgIn[5], (char *)&szLineBuf[10]);
									func->szArgIn[5][strlen(func->szArgIn[5])-1]=0;
								}
							}
							else if( !memcmp(szLineBuf, "TIMEVAL:", 8) )
							{
								func->iTimerValue = atoi((char*)&szLineBuf[8]);
							}
							else if( !memcmp( szLineBuf, "DLLNAME:", 8) )
							{
								wsprintf( func->dll_name, (char *)&szLineBuf[8]);
								func->dll_name[strlen(func->dll_name) -1 ] = 0;
							}
							else if( !memcmp( szLineBuf, "FUNCTION:", 9) )
							{
								memset( func->function_name, 0, 64);
								wsprintf( func->function_name, (char *)&szLineBuf[9]);
								func->function_name[strlen(func->function_name)-1] = 0;
							}
							else if( !memcmp(szLineBuf, "FALSEGO:", 8) )
							{
								func->falsegoto = atoi( (char *)&szLineBuf[8]);
							}
							else if( !memcmp(szLineBuf, "FILENAME:", 9) )
							{
								wsprintf(func->dll_name, (char*)&szLineBuf[9]);
							}
							else if( !memcmp(szLineBuf, "MAXSIZE:", 8) )
							{
								func->iMaxSize = atoi( (char *)&szLineBuf[8]);
							}
							else if( !memcmp(szLineBuf, "STOPUINPUT:", 11) )
							{
								func->iStopUserInput = atoi( (char*)&szLineBuf[11]);
							}
							else if( !memcmp(szLineBuf, "DIRECT:", 7) )
							{
								func->iDirect = atoi( (char*)&szLineBuf[7]);
							}
						}
						else if( stepdATA.type == 7)
						{
							if(!memcmp(szLineBuf,"LIST:", 5) )
							{
								int isize = strlen(&szLineBuf[5]);
								for(int ic = 0; ic < isize ; ic++ )
								{
									if( szLineBuf[5+ic] == '\n' ||
										szLineBuf[5+ic] == '\r' )
										szLineBuf[5+ic] = 0;
								}
								wsprintf(func->dll_name,"%s", &szLineBuf[5]);
							}
						}
						else if( stepdATA.type == 10)
						{
							time = &stepdATA.sub_item.time;
							if( !memcmp( szLineBuf, "DAILYCHK:", 9) )
							{
								time->bDailyChk = atoi(&szLineBuf[9]);
							}
							else if( !memcmp(szLineBuf, "SPECIALDAYCHK:", 14))
							{
								time->bSepecialDayChk = atoi(&szLineBuf[14]);
							}
							else if( !memcmp(szLineBuf, "ICHK_GO:", 8) )
							{
								time->iChk_go = atoi(&szLineBuf[8]);
							}
							else if( !memcmp(szLineBuf, "DAILYCHK_GO:", 12) )
							{
								time->iDailyChk_go = atoi(&szLineBuf[12]);
							}
							else if( !memcmp(szLineBuf, "DAILYCHK_FNAME:", 15) )
							{
								int i,iSize;
								sprintf(time->dfname, "%s", &szLineBuf[15]);
								iSize = strlen(time->dfname);
								for( i = 0; i< iSize; i++)
								{
									if( time->dfname[i] == '\r')
										time->dfname[i] = 0;
									if( time->dfname[i] == '\n')
										time->dfname[i] = 0;
								}
							}
							else if( !memcmp(szLineBuf, "SPECIALDAYCHK_FNAME:", 20 ))
							{
								int i,iSize;
								sprintf(time->sfname, "%s", &szLineBuf[20]);
								iSize = strlen(time->sfname);
								for( i = 0; i< iSize; i++)
								{
									if( time->sfname[i] == '\r')
										time->sfname[i] = 0;
									if( time->sfname[i] == '\n')
										time->sfname[i] = 0;
								}
							}
							//else if( !memcmp(szLineBuf, "ONTIMESTARTHOUR:", 16) )
							//{
							//	time->beginHour = atoi(&szLineBuf[16]);
							//}
							//else if( !memcmp(szLineBuf, "ONTIMESTARTMIN:", 15) )
							//{
							//	time->beginMin = atoi(&szLineBuf[15]);
							//}
							//else if( !memcmp(szLineBuf, "OFFTIMESTARTHOUR:", 17) )
							//{
							//	time->endHour = atoi(&szLineBuf[17]);
							//}
							//else if( !memcmp(szLineBuf, "OFFTIMESTARTMIN:", 16) )
							//{
							//	time->endMin = atoi(&szLineBuf[16]);
							//}
						}
						else if( stepdATA.type == 6)
						{
							item_hub * pHub = &stepdATA.sub_item.hub;
							if( !memcmp(szLineBuf, "MPOINT:", 7 ))
							{
								pHub->iTogo[pHub->iTogoCount++] = atoi(&szLineBuf[7]);
							}
							else if( !memcmp(szLineBuf, "MDTMF:", 6 ))
							{
								pHub->iDtmf[pHub->iDtmfCount++] = atoi(&szLineBuf[6]);
							}
							else if( !memcmp(szLineBuf, "UINPUT:", 7 ))
							{
								pHub->iCompDtmfBufNum = atoi(&szLineBuf[7]);
							}
							else if ( memcmp(szLineBuf, "FALSEGO:", 8) == NULL )
							{
								pHub->falsegoto = atoi( (char *)&szLineBuf[8]);
							}
						}
					}
					memset( szLineBuf, 0, 256);
					inbuf = fgets( szLineBuf, 256, fp);
				}
				fclose(fp);
			}
		}
	}
	for( int i = 0; i< iMaxAllowedCall ; i++)
	{
		scm[i].iScenarioPageSum = sf_count;
	}

	return TRUE;
}

BOOL InitScenarioManager()
{
	if( FindScenarioFile() )
	{
		scm = new SCM[iMaxAllowedCall];
		if( scm )
		{
			for( int i = 0; i < iMaxAllowedCall; i++)
			{
				scm[i].m_pCon = &pCon[i];
				scm[i].iCallNum = i;
				scm[i].iInstalledLang = iInstalledLang;
				memset(&scm[i].billingPacket.pBody, 0x20, sizeof(IVR_AUTHENTICATION_PACKET_BODY));
			}
			bLoadScenarioFile();
			return TRUE;
		}
	}
	return FALSE;
}
