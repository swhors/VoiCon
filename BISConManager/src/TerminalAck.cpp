// TerminalAck.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
//#include "IVRRemoteCtrlDlg.h"
#include "Terminal.h"
#include "TerminalAck.h"
#include "TerminalDef.h"
#include "msg_define.h"
#include "Globaldef.h"

#include "BISConManager.h"
#include "BISConManagerDoc.h"
#include "StateView.h"

#include "ioctrl.h"
#include "ValInterface.h"
#include "FileInterface.h"

extern CBISConManagerApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 1
void CALLBACK ReStartIVR(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime);
#endif

char * requestFunc[20]=
{
	"gkIP", 
	"gkPort", 
	"localQPort", 
	"localRPort", 
	"h323Id", 
	"e164Id",
	"EnableDebugConsole",
	"EnableCallCouple",
	"reponseTimeout",
	"InputWaitTime",
	"InputInitWaitTime",
	"retsetCall"
};
char *preFix[21]=
{
	"default gk ip:",
	"default gk port:",
	"default call port:",
	"default ras port:",
	"h323id:",
	"e164id:",
	"enableDebugConsole:",
	"enableCallCoupling:",
	"ttl:",
	"DTMF Input Delay:",
	"User Input Wait:"
};

char gk_conf_string[13][20] = 
{
#ifdef __ACROMATE__IVR
	"gkip",				// 0
	"gkport",			// 1
	"h323id",			// 2
	"e164",				// 3
	"debugcon",			// 4
	"callcoupl",		// 5
	"ttl",				// 6
	"delay",			// 7
	"scenario",			// 9
	"language",			// 8
	"userinputdelay",	// 10
	"userinputwait",	// 11
	" "					// 12
#else
	"gkip",				// 0
	"gkport",			// 1
	"localQPort",		// 2
	"localRPort",		// 3
	"h323id",			// 4
	"e164",				// 5
	"debug",			// 6
	"callcoupl",		// 7
	"ttl",				// 8
	"userinputdelay",	// 9
	"userinputwait",	// 10
	"scenario",			// 11
	"language",			// 12
#endif
};


/////////////////////////////////////////////////////////////////////////////
// CTerminalAck

CTerminalAck::CTerminalAck()
{
	iTerminalNum = 0;
	initValue();
}

CTerminalAck::~CTerminalAck()
{
}

void LogWriteTerminalOperate(char * slog0, char * slog1, int i0, int i1);

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CTerminalAck, CAsyncSocket)
	//{{AFX_MSG_MAP(CTerminalAck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CTerminalAck member functions

void CTerminalAck::OnReceive(int nErrorCode) 
{
	char sbuf[8];
	char Endbuf[]={13,10};
	memset( sbuf, 0, 8);
	int iLen = Receive(sbuf, 8, 0);
	if( iLen < 0){
		initValue();
		Close();
	}
	if( iLen >0)
	{
		switch(sbuf[0])
		{
		case KEY_CTRL_C:
			Send(KEY_CTRL_C_STR, 2, 0);
			initValue();
			Close();
			return;
		case KEY_BACKSPACE:
			{
				CString csTbuf;
				char * tbuf;
				int iLen = strlen(sReceiveBuf);
				if( strlen(sReceiveBuf) > 0)
				{
					if( iLeft > 0)
					{
						for( int iPos = iLen - iLeft; iPos < iLen ; iPos++)
						{
							sReceiveBuf[iPos++] = sReceiveBuf[iPos];
						}
					}
					sReceiveBuf[iLen-1] = 0;
					tbuf = (char *)malloc(iLen+1);
					memset( tbuf, 0x20, iLen);
					tbuf[iLen] = 0;
					csTbuf.Format("\r>%s", tbuf);
					Send(csTbuf,csTbuf.GetLength(),0);
					free(tbuf);
					csTbuf.Format("\r>%s", sReceiveBuf);
					Send(csTbuf,csTbuf.GetLength(),0);
					return;
				}
			}
			return;
		case KEY_ARROW:
			{ 
				int iSkip = 0;
				CString sendbuf;
				char *tbuf;
				int iLen = 0;
				switch(sbuf[2]){
				case KEY_UP:
					iSkip = 1;
					if( --iCommanBuffer <0 )
						iCommanBuffer = 10;
				case KEY_DOWN:
					if( iSkip == 0)
					{
						if( ++iCommanBuffer >=10 )
							iCommanBuffer = 0;
					}
					iLen = strlen( command_history[iCommanBuffer]);
					if( iLen > 0)
					{
						iLen = strlen(sReceiveBuf)+1;
						tbuf = (char *) malloc(iLen);
						memset( tbuf, 0x20, iLen);
						tbuf[iLen-1] = 0;
						sendbuf.Format("\r>%s", tbuf);
						Send(sendbuf, sendbuf.GetLength(), 0);
						free(tbuf);
						memset( sReceiveBuf, 0, 1024);
						sprintf(sReceiveBuf, command_history[iCommanBuffer], sReceiveBuf);
						sendbuf.Format("\r>%s", sReceiveBuf);
						Send(sendbuf, sendbuf.GetLength(), 0);
					}
					return;
				case KEY_LEFT:
					if( iLeft < strlen(sReceiveBuf))
						iLeft++;
					break;
				case KEY_RIGHT:
					if( iLeft>0 )
						iLeft--;
					break;
				}
			}
		default:
#ifdef _AUTH_TERMINAL
			if( !bUserPw && bUserID )
			{
				;
			}
			else
			{
				if( !bInputPasswd )
					Send(sbuf, strlen( sbuf));
			}
#else //_AUTH_TERMINAL
			Send(sbuf, strlen( sbuf));
#endif // _AUTH_TERMINAL
			
			if( memcmp( sbuf, END_LINE, 2) == 0)
			{
				if( bInputPasswd )
				{
					ModifyPassword();
					return;
				}
				
#ifdef _AUTH_TERMINAL
				if( !bUserID || ! bUserPw)
				{
					if(! bUserID )
					{
						if( bVerifyUser() == FALSE)
						{
							bUserID = FALSE;
							if( iRetrialCount++ == 4)
							{
								initValue();
								//Send(DISCONNECTED, strlen(DISCONNECTED));
								Close();
							}
							memset(szUserID, 0, 32);
							memset(szUserPW, 0, 32);
							Send(LOGIN_INCORRECT, strlen(LOGIN_INCORRECT));
							Send(LOGIN, strlen(LOGIN));
						}
						else
						{
							bUserID = TRUE;
							Send(PASSWORD, strlen(PASSWORD));
						}
						memset(sReceiveBuf, 0, 1024);
						return;
					}
					if(! bUserPw )
					{
						if( bVerifyPasswd() == FALSE)
						{
							bUserID = FALSE;
							if( iRetrialCount++ == 4)
							{
								initValue();
								//Send(DISCONNECTED, strlen(DISCONNECTED));
								Close();
							}
							memset(szUserID, 0, 32);
							Send("\r\n", 2);
							Send(LOGIN_INCORRECT, strlen(LOGIN_INCORRECT));
							Send(LOGIN, strlen(LOGIN));
						}
						else
						{
							Send("\r\n>", 3);
							bUserPw = TRUE;
						}
						memset(szUserPW, 0, 32);
						memset(sReceiveBuf, 0, 1024);
						return;
					}
				}
#endif//_AUTH_TERMINAL
				if( strlen( sReceiveBuf ) == 0)
				{
					if( bDetailLog == TRUE)
					{
						bDblEnterDlog = TRUE;
						bDetailLog = FALSE;
					}
					else
					{
						if( bDblEnterDlog == TRUE)
						{
							if( bDetailLog == FALSE)
								bDetailLog = TRUE;
							bDblEnterDlog = FALSE;
						}
					}
					if( bCallLog == TRUE)
					{
						bDblEnterClog = TRUE;
						bCallLog = FALSE;
					}
					else
					{
						if( bDblEnterClog == TRUE)
						{
							if( bCallLog == FALSE)
								bCallLog = TRUE;
							bDblEnterClog = FALSE;
						}
					}
				}
				
				iLeft = 0;
				if( ParseCommand(sReceiveBuf) == 0)
				{
					if( strlen( sReceiveBuf) > 0)
					{
						if( iCommanBuffer < 10)
						{
							memset( command_history[iCommanBuffer], 0, 1024);
							sprintf(command_history[iCommanBuffer++], sReceiveBuf);
						}
						else
						{
							iCommanBuffer = 0;
							memset( command_history[iCommanBuffer], 0, 1024);
							sprintf(command_history[iCommanBuffer++], sReceiveBuf);
						}
					}
					memset(sReceiveBuf, 0, 1024);
					if( !bInputPasswd)
						Send(">", 2, 0);
				}
				else
				{
					initValue();
					//Send(DISCONNECTED, strlen(DISCONNECTED));
					Close();
				}
			}
			else
			{
				if( iLeft > 0 && iLeft <=80)
				{
					if( sbuf[0] != KEY_ARROW)
					{
						sReceiveBuf[strlen(sReceiveBuf) - iLeft--] = sbuf[0];
					}
				}
				else
				{
					strcat(sReceiveBuf, sbuf);
				}
			}
			break;
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

int CTerminalAck::ParseCommand(char *sCommand)
{
	int iRet = 0;
	CString csCommand = sCommand;
	CString csResponse;
	if( csCommand.GetLength() <= 0)
	{
		return iRet;
	}
	csCommand.MakeLower();
	if( !memcmp(csCommand, COMMAND_EXIT, strlen( COMMAND_EXIT))
		||!memcmp(csCommand, "quit", 4))
	{
		csResponse = "Bye.\r\n";
		iRet = -1;
	}
	else if( !memcmp(csCommand, COMMAND_1_HELP, strlen( COMMAND_1_HELP)))
	{
		CommandHelpParse( sCommand, csResponse);
	}
	else if( !memcmp(csCommand, COMMAND_2_VIEW, strlen( COMMAND_2_VIEW)))
	{
		CommandViewParse(sCommand,csResponse);
	}
	else if( !memcmp(csCommand, COMMAND_3_IVR, strlen(COMMAND_3_IVR)))
	{
		CommandIvrParse(sCommand, csResponse);
	}
	else if( !memcmp(csCommand, COMMAND_4_LOG, strlen(COMMAND_4_LOG)))
	{
		CommandLogParse(sCommand, csResponse);
	}
	else if( !memcmp(csCommand, "ftp", 3))
	{
		CommandServiceParse(sCommand);
	}
	else if( !memcmp(csCommand, COMMAND_3_SETPWD, strlen(COMMAND_3_SETPWD)))
	{
		AdminUser(1, csCommand, csResponse);
	}
	else if( !memcmp(csCommand, COMMAND_3_ADDU, strlen(COMMAND_3_ADDU)))
	{
		AdminUser(2, csCommand, csResponse);
	}
	else
	{
		csResponse = "Unknown Command\r\n";
	}
	Send(csResponse, csResponse.GetLength(), 0);

	return iRet;
}

void CTerminalAck::OnClose(int nErrorCode) 
{
	initValue();
	Close();
	//CAsyncSocket::OnClose(nErrorCode);
}


void CTerminalAck::CommandViewParse(char *sArg,CString &csResponse)
{
	CString csArg = (char *)&sArg[strlen(COMMAND_2_VIEW)+1];
	csArg.MakeLower();
	if( !csArg.Compare(COMMAND_2_CONF) )
	{
		ViewConfigure(csResponse);
	}
	else if( !csArg.Compare(COMMAND_2_SC_L) )
	{
		ViewScenarioFileList(csResponse);
	}
	else if( !memcmp(csArg,COMMAND_2_SCE, strlen(COMMAND_2_SCE)) )
	{
		ViewScenarioFile(csArg);
	}
}

void CTerminalAck::CommandHelpParse(char *sArg,CString &csResponse)
{
	CString csArg = (char *)&sArg[strlen(COMMAND_1_HELP)+1];
	csArg.MakeLower();
	if( !memcmp(csArg, COMMAND_2_VIEW, strlen( COMMAND_2_VIEW)))
	{
		csResponse = "View configure        : view ivr configuration file.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "View Scenario \"Number\": view number'th scenario file.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "View Call \"Number\"    : view call state of number'th call.\r\n";
	}
	else if( !memcmp(csArg, COMMAND_3_IVR, strlen( COMMAND_3_IVR)))
	{
		char * tbuf = (char*)strstr(csArg, " ");
		if( tbuf )
		{
			tbuf = tbuf+1;
			if( tbuf )
			{
				if( memcmp( tbuf, COMMAND_3_SET, strlen( COMMAND_3_SET))==0)
				{
					csResponse = "ivr set gkip         : set gatekeeper ip address.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set gkport       : set gatekeeper port.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set h323id       : set ivr's h323 id.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set e164         : set ivr's e164.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set delay        : set ivr's audio delay jitter.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set ttl          : set ivr's ttl.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set callCoupl    : Set value which ivr will allow outbound call [TRUE/FALSE].\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set UserInputDelay: Set user input time delay.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set scenario num : set num'th ivr scenario.\r\n";
					Send(csResponse, csResponse.GetLength(), 0);
					csResponse = "ivr set language num : set num'th ivr language.\r\n";
					return;
				}
			}
		}
		csResponse = "ivr start     : Start ivr program.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "ivr stop      : Stop running Ivr program.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "ivr isrun     : Check ivr's run state.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "ivr set item  : set ivr's configuration item.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "ivr adduser   : add manager.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "ivr passwd    : set manager's passwd.\r\n";
	}
	else if( !memcmp(csArg, COMMAND_4_LOG, strlen( COMMAND_3_IVR)))
	{
		csResponse = "log calllog   [TRUE/FALSE] : Always view call log.\r\n";
		Send(csResponse, csResponse.GetLength(), 0);
		csResponse = "log detaillog [TRUE/FALSE] : Always view ivr detail log.\r\n";
	}
	else
	{
		csResponse = "help ivr\r\nhelp view\r\nhelp log\r\n";
	}
}

void CTerminalAck::CommandIvrParse(char *sArg, CString &csResponse,int iVal)
{
	//CIVRRemoteCtrlDlg * pDlg = (CIVRRemoteCtrlDlg * )AfxGetApp()->m_pMainWnd;
	CString csArg = (char *) &sArg[strlen(COMMAND_3_IVR)+1];
	char sbuf[300];
	csArg.MakeLower();
	BOOL bLStart = theApp.bFindIvrProc();
	HWND hWnd = theApp.m_pMainWnd->m_hWnd;
	//HWND hWnd = ::GetActiveWindow();

	if( csArg.Compare(COMMAND_3_STATE) == 0)
	{
		SendMessage(hWnd, theApp.MessageRes, ACRO_IVR_GETCONSTATE_REQ, 2);
	}
	else if( csArg.Compare(COMMAND_3_ISRUN) == 0)
	{
		SendMessage(hWnd, theApp.MessageRes, ACRO_IVR_GETRUNSTATE_ACK, bLStart);
	}
	else if(!csArg.Compare(COMMAND_3_UPSTART) || !csArg.Compare(COMMAND_3_UP) || csArg.Compare(COMMAND_3_START) == 0 )
	{
		if( !theApp.bFindIvrProc() )
		{
			theApp.SetIVRStart();
			//theApp.bForceDrop = FALSE;
			//theApp.bForceEnded = FALSE;
			//theApp.bAutoStart = TRUE;
			//if( !theApp.bSetTimer )
			//{
			//	SetTimer(hWnd, 90000, 500, ReStartIVR);
			//}
			//else
			//{
			//	theApp.bForceDrop = FALSE;
			//	theApp.bForceEnded = FALSE;
			//}
			if( AfxIsValidAddress(theApp.terminal, sizeof( CTerminal ), TRUE))
			{
				theApp.terminal->SendCallLog("OK", -1);
			}
		}
		else
		{
			if( AfxIsValidAddress(theApp.terminal, sizeof( CTerminal ), TRUE))
			{
				theApp.terminal->SendCallLog("Error : Already run.", -1);
			}
		}
	}
	else if(csArg.Compare(COMMAND_3_DOWNKILL) == 0 || csArg.Compare(COMMAND_3_DOWN) == 0
		|| csArg.Compare(COMMAND_3_STOP) == 0)
	{
		theApp.bForceEnded = TRUE;
		if( theApp.bFindIvrProc() != TRUE)
		{
			if( AfxIsValidAddress(theApp.terminal, sizeof( CTerminal ), TRUE))
			{
				theApp.terminal->SendCallLog("Error : BIS_IVR is not runnig state.", -1);
			}
		}
		else
		{
			theApp.SetIVRStop();
		}
	}
	else
	{
		memset( sbuf, 0, 300);
		sprintf( sbuf, csArg);
		int iLen = strlen( sbuf);
		if( memcmp(sbuf, COMMAND_3_SET, strlen(COMMAND_3_SET)) == 0)
		{
			csArg = csArg.Right(iLen - strlen( COMMAND_3_SET) - 1);
			SetConfigure( csArg, csResponse);
		}
	}
}

//이 함수는 기존의 아크로 버젼의 소스를 라드 호완 버젼으로 변경하기 위한
//중간 역활의 소스임.
int MakeCfgData(CTerminalAck & this_send)
{
	char ttBuf[128];
	//OpenVal();
	for( int i = 0; i < 10; i++)
	{
		int iLen =0;
		if( i == 0 || i == 4 || i == 5 )
		{
			char tbuf[64];
			memset(tbuf, 0, 64);
			iLen = 64;
			ConfRead(requestFunc[i], tbuf, &iLen,TRUE);
			sprintf(ttBuf, "%s%s\r\n", preFix[i], tbuf);
		}
		else
		{
			iLen = 0;
			ConfRead(requestFunc[i], NULL, &iLen,FALSE);
			sprintf(ttBuf, "%s%d\r\n", preFix[i], iLen);
		}
		this_send.Send(ttBuf, strlen(ttBuf), 0);
	}
	//CloseVal();
	return 0;
}

int WriteToFileData(IN char * fFile, IN char * dData)
{
	char DataBuf[10][64];
	int i =0, iNum = 0;
	FILE *fp;
	fp = fopen(fFile, "r+t");
	
	if( !fp )
	{
		return 0;
	}
	
	for( i = 0 ; i < 10; i++)
	{
		memset(DataBuf[i], 0, 64);
	}
	i = 0;
	
	while(fgets(DataBuf[i++], 64, fp)!= NULL)
	{
		;
	}
	fclose(fp);
	//DeleteFile(fFile);
	fp = fopen(fFile, "w+t");
	if( !fp )
		return 0;
	iNum = atoi(dData);
	if( iNum>= 0 && iNum < 10)
	{
		char *tbuf = (char *)&dData[2];
		memcpy(DataBuf[iNum], tbuf, strlen(tbuf));
	}
	
	for( i = 0; i < 10; i++)
	{
		if( strlen( DataBuf[i]) >0)
		{
			fwrite(DataBuf[i], strlen( DataBuf[i]), 1, fp);
		}
	}
	fclose(fp);
	return 1;
}

//
int SendFileData(IN char *fFile,IN char *appendStr , CTerminalAck & this_send)
{
	//CString csResponse;
	FILE *fp;
	fp = fopen(fFile, "r+t");
	if( !fp )
	{
		return 0;
	}
	char sbuf[1024], *tbuf;
	memset(sbuf, 0, 1024);
	int iSkipLength = 0, iLineBufLength=0;
	if( appendStr )
	{
		iSkipLength = strlen(appendStr);
		memcpy(sbuf, appendStr, iSkipLength);
		iLineBufLength = 1024-iSkipLength;
	}
	else
	{
		iLineBufLength = 1024;
	}
	while((tbuf = fgets((char*)&sbuf[iSkipLength], iLineBufLength, fp)) != NULL)
	{
		char * tttbuf=strstr(sbuf, "\n");
		if( tttbuf ) strcat(sbuf, "\r");
		else strcat(sbuf, "\n\r");
		this_send.Send(sbuf, strlen(sbuf), 0);
		memset( (char *)&sbuf[iSkipLength], 0, iLineBufLength);
	}
	fclose(fp);
	return 1;
}

//BIS_IVR의 환경 설정 값을 전송함.

void CTerminalAck::ViewConfigure(CString &csResponse)
{
#ifdef __ACROMATE__IVR
	// this routine is for acromate.
	FILE * fp = fopen("AcroIvr.cfg", "rt");

	if( !fp )
	{
		csResponse = ERROR_NOT_OPEN_CONF;
		return;
	}
	char sbuf[1024], *tbuf;
	memset(sbuf, 0, 1024);
	csResponse = "SEND START\r\n";
	while((tbuf = fgets(sbuf, 1024, fp)) != NULL)
	{
		csResponse = sbuf;
		if( csResponse.Find("\n", 0))
		{
			strcat(sbuf, "\r");
		} else {
			strcat(sbuf, "\n\r");
		}
		Send(sbuf, strlen(sbuf), 0);
		memset( tbuf, 0, 1024);
	}
	fclose(fp);
	csResponse = "SEND END\r\n";
#else //__ACROMATE__IVR
	//
	// this routine is for radvision.
	csResponse = "SEND START\r\n";
	MakeCfgData(*this);
	SendFileData("scenario.ini", "SCENARIO:", *this);
	SendFileData("language.ini", "LANGUAGE:", *this);
	csResponse = "SEND END\r\n";
#endif //__ACROMATE__IVR
	return;
}

void CTerminalAck::SetConfigure(CString csItem, CString &csResponse)
{
#ifdef __ACROMATE__IVR
	char sbuf[260], *tbuf;
	int iNum = 0;
	memset( sbuf, 0, 260);
	sprintf( sbuf, csItem);
	for( int i = 0; i < 13; i++)
	{
		if( memcmp( sbuf, gk_conf_string[i], strlen(gk_conf_string[i]))==0)
		{
			// ip
			if( i == 8 || i == 9)
			{
				char * ttbuf = (char *)&sbuf[strlen(gk_conf_string[i])+1];
				iNum = atoi(ttbuf);
				tbuf = strstr(ttbuf, " ");
				WriteConf( i, tbuf+1, iNum, csResponse);
				return;
			}
			else if ( i == 4 || i == 5 )
			{
				memset( sbuf, 0, 260);
				csItem.MakeUpper();
				sprintf( sbuf, csItem);
				WriteConf( i, (char *)&sbuf[strlen(gk_conf_string[i])+1], iNum, csResponse);
				return;
			}
			else 
			{
				WriteConf( i, (char *)&sbuf[strlen(gk_conf_string[i])+1], iNum, csResponse);
				return;
			}
		}
	}
	csResponse=ERROR_UNKOWN_COMMAND;
#else // __ACROMATE__IVR
	char sbuf[260];
	int iNum = 0;
	memset( sbuf, 0, 260);
	sprintf( sbuf, csItem);
	for( int i = 0; i < 13; i++)
	{
		char *conf_lower;
		conf_lower = strlwr(strdup(gk_conf_string[i]));
		if( memcmp( sbuf, gk_conf_string[i], strlen(gk_conf_string[i]))==0
			|| memcmp( sbuf, conf_lower, strlen(conf_lower))==0 )
		{
			if( i <= 9 )
			{
				char * ttbuf = (char *)&sbuf[strlen(gk_conf_string[i])+1];
				int iLength = 0;
				//i ==6 or 8 or 9인 경우는 profile에 데이터를 기록함.
				//따라서, Val 핸들을 필요로 하지 않음.
				//if( i != 6 && i != 8 && i != 9 ) OpenVal();
				
				if( i == 0 || i == 4 || i == 5)
				{
					iLength = strlen(ttbuf);
					ConfWrite(requestFunc[i], ttbuf, &iLength, TRUE);
				}
				else
				{
					iLength = atoi(ttbuf);
					ConfWrite(requestFunc[i], NULL, &iLength, FALSE);
				}
				
				//i ==6 or 8 or 9인 경우는 profile에 데이터를 기록함.
				//따라서, Val 핸들을 필요로 하지 않음.
				//if( i != 6 && i != 8 && i != 9 ) CloseVal();
				
				return;
			}
			else if( i == 11)
			{
				char * ttbuf = (char *)&sbuf[strlen(gk_conf_string[i])+1];
				WriteToFileData("scenario.ini", ttbuf);
				return;
				// 시나리오
			}
			else if( i == 12)
			{
				char * ttbuf = (char *)&sbuf[strlen(gk_conf_string[i])+1];
				WriteToFileData("language.ini", ttbuf);
				return;
				// 언어
			}
		}
	}
	csResponse=ERROR_UNKOWN_COMMAND;
#endif // __ACROMATE__IVR
}

typedef struct tag_ACROCONF
{
	char gkip[48]; // 0
	char gkport[12]; // 1
	char h323id[128]; // 2
	char e164[128]; // 3
	char couple[6]; // 4
	char ttl[8]; // 6
	char delay[6]; // 7
	char scenario[10][256]; // 8
	int iScenario;
	char LANGUAGE[4][5]; // 9
	char userinputdelay[12];//10
	char userinputwait[12]; //12
	int iLanguage;
} ACROCONF;

void CTerminalAck::WriteConf(int iItem, char *pArg, int iReserved, CString & csResponse)
{
	FILE * fp = fopen("AcroIvr.cfg", "rt");
	if( !fp )
	{
		csResponse = ERROR_NOT_OPEN_CONF;
		return;
	}
	
	char sbuf[1024], *tbuf;
	ACROCONF conf;
	memset ( & conf, 0, sizeof( ACROCONF ));
	conf.iLanguage = 0;
	conf.iScenario = 0;
	memset(sbuf, 0, 1024);
	while((tbuf = fgets(sbuf, 1024, fp)) != NULL)
	{
		tbuf[strlen(tbuf)-1] = 0;
		if( memcmp( tbuf, ITEM_GK_IP_S, strlen( ITEM_GK_IP_S)) == 0)
		{
			sprintf( conf.gkip, (char *)&tbuf[strlen(ITEM_GK_IP_S)]);
		}
		else if ( memcmp( tbuf, ITEM_GK_PORT_S, strlen( ITEM_GK_PORT_S)) == 0)
		{
			sprintf( conf.gkport, (char *)&tbuf[strlen(ITEM_GK_PORT_S)]);
		}
		else if ( memcmp( tbuf, ITEM_E164_S, strlen( ITEM_E164_S)) == 0)
		{
			sprintf( conf.e164, (char *)&tbuf[strlen(ITEM_E164_S)]);
		}
		else if ( memcmp( tbuf, ITEM_H323ID_S, strlen( ITEM_H323ID_S)) == 0)
		{
			sprintf( conf.h323id, (char *)&tbuf[strlen(ITEM_H323ID_S)]);
		}
		else if ( memcmp( tbuf, ITEM_TTL_S, strlen( ITEM_TTL_S)) == 0)
		{
			sprintf( conf.ttl, (char *)&tbuf[strlen(ITEM_TTL_S)]);
		}
		else if ( memcmp( tbuf, ITEM_LANGUAGE_S, strlen( ITEM_LANGUAGE_S)) == 0)
		{
			sprintf( conf.LANGUAGE[conf.iLanguage++], (char *)&tbuf[strlen(ITEM_LANGUAGE_S)]);
		}
		else if ( memcmp( tbuf, ITEM_SCENARIO_S, strlen( ITEM_SCENARIO_S)) == 0)
		{
			sprintf( conf.scenario[conf.iScenario++], (char *)&tbuf[strlen(ITEM_SCENARIO_S)]);
		}
		else if ( memcmp( tbuf, ITEM_DELAY_S, strlen( ITEM_DELAY_S)) == 0)
		{
			sprintf( conf.delay, (char *)&tbuf[strlen(ITEM_DELAY_S)]); //9410858743711030
		}
		else if ( memcmp( tbuf, ITEM_CALL_COUPLE_S, strlen( ITEM_CALL_COUPLE_S)) == 0)
		{
			sprintf( conf.couple, (char *)&tbuf[strlen(ITEM_CALL_COUPLE_S)]);
		}
		else if ( memcmp( tbuf, ITEM_USERINPUTDELAY_S, strlen(ITEM_USERINPUTDELAY_S)) == 0)
		{
			sprintf( conf.userinputdelay, (char *)&tbuf[strlen(ITEM_USERINPUTDELAY_S)]);
		}
		else if ( memcmp( tbuf, ITEM_USERINPUTWAIT_S, strlen(ITEM_USERINPUTWAIT_S)) == 0)
		{
			sprintf( conf.userinputwait, (char *)&tbuf[strlen(ITEM_USERINPUTWAIT_S)]);
		}
		memset(sbuf, 0, 1024);
	}
	fclose(fp);
	switch(iItem)
	{
	case 0: // gkip
		memset( conf.gkip, 0, 48);
		sprintf( conf.gkip, pArg);
		break;
	case 1: // gkport
		memset( conf.gkport, 0, 12);
		sprintf( conf.gkport, pArg);
		break;
	case 2: // h323 id
		memset( conf.h323id, 0, 128);
		sprintf( conf.h323id, pArg);
		break;
	case 3: // e164 id
		memset( conf.e164, 0, 128);
		sprintf( conf.e164, pArg);
		break;
	case 4: // call coupl
		memset( conf.couple, 0, 6);
		sprintf( conf.couple, pArg);
		break;
	case 6: // // ttl
		memset( conf.ttl, 0, 8);
		sprintf( conf.ttl, pArg);
		break;
	case 7: // delay
		memset( conf.delay, 0, 6);
		sprintf( conf.delay, pArg);
		break;
	case 8: // scenario
		{
			if( conf.iScenario >= iReserved && iReserved < 10)
			{
				memset( conf.scenario[iReserved], 0, 256);
				sprintf( conf.scenario[iReserved], pArg);
				if( iReserved >= conf.iScenario)
					conf.iScenario++;
			}
			else
			{
				csResponse = ERROR_CONF_ERROR_1;
				return;
			}
		}
		break;
	case 9: // language
		{
			if( strlen( pArg ) == 2)
			{
				if( conf.iLanguage >= iReserved && iReserved < 4 && conf.iLanguage> -1)
				{
					memset( conf.LANGUAGE[iReserved], 0, 4);
					sprintf( conf.LANGUAGE[iReserved], pArg);
				}
				else
				{
					csResponse = ERROR_CONF_ERROR_2;
					return;
				}
			}
			else
			{
				csResponse = ERROR_CONF_ERROR_3;
				return;
			}
		}
		break;
	case 10:
		{
			memset( conf.userinputdelay, 0, 12);
			sprintf( conf.userinputdelay, pArg);
		}
	case 11:
		{
			memset(conf.userinputwait, 0, 12);
			sprintf( conf.userinputwait, pArg);
		}
		break;
	}
	fp = fopen( "AcroIvr.cfg", "wt");
	if( !fp )
	{
		csResponse = ERROR_CONF_ERROR_4;
		return;
	}
	CString csTbuf;
	csTbuf = "#this is acroivr configuration file\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	csTbuf = "[GATEKEEPER]\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = ITEM_GK_IP_S;
	csTbuf += conf.gkip;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = ITEM_GK_PORT_S;
	csTbuf += conf.gkport;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = "[TERMINAL]\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = ITEM_H323ID_S;
	csTbuf += conf.h323id;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = ITEM_E164_S;
	csTbuf += conf.e164;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = ITEM_CALL_COUPLE_S;
	csTbuf += conf.couple;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = ITEM_TTL_S;
	csTbuf += conf.ttl;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	
	csTbuf = ITEM_DELAY_S;
	csTbuf += conf.delay;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);

	csTbuf = ITEM_USERINPUTDELAY_S;
	csTbuf += conf.userinputdelay;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);

	csTbuf = ITEM_USERINPUTWAIT_S;
	csTbuf += conf.userinputwait;
	csTbuf +="\n";
	fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
		
	for( int i = 0; i < conf.iScenario ; i++)
	{
		csTbuf = ITEM_SCENARIO_S;
		csTbuf += conf.scenario[i];
		csTbuf +="\n";
		fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	}
	for( int b = 0; b < 4; b++)
	{
		csTbuf = ITEM_LANGUAGE_S;
		csTbuf += conf.LANGUAGE[b];
		csTbuf +="\n";
		fwrite( csTbuf, 1, csTbuf.GetLength(), fp);
	}
	fclose(fp);
	csResponse = "OK\r\n";
}

void LogWriteTerminalOperate(char * slog0, char * slog1, int i0, int i1)
{
	FILE * fp = NULL;
	fp = fopen("Terminal.log", "a+t");
	char sbuf[128];
	if( fp )
	{
		memset(sbuf, 0, 128);
		sprintf(sbuf, "%s %s %4d %4d\n", slog0, slog1, i0, i1);
		fwrite( sbuf, strlen( sbuf), 1, fp);
		fclose(fp);
	}
}

void CTerminalAck::CommandLogParse(char *sArg, CString &csResponse)
{
	CString csArg = strstr(sArg, " ") +1;
	char * tbuf = NULL;
	tbuf = (char*)strstr(csArg, " ");
	
	if( memcmp( csArg, COMMAND_4_CLOG, strlen( COMMAND_4_CLOG)) == 0)
	{
		if( tbuf )
		{
			tbuf = tbuf+1;
			csArg = tbuf;
			csArg.MakeLower();
			if( tbuf )
			{
				if( !csArg.Compare("true"))
				{
					bCallLog = TRUE;
				}
				else
				{
					bCallLog = FALSE;
					if( bDblEnterClog == TRUE)
					{
						bDblEnterClog = FALSE;
					}
				}
				csResponse = "OK\r\n";
				return;
			}
		}
	}
	else if ( memcmp( csArg, COMMAND_4_DLOG, strlen( COMMAND_4_DLOG)) == 0)
	{
		if( tbuf )
		{
			tbuf = tbuf+1;
			csArg = tbuf;
			csArg.MakeLower();
			if( tbuf )
			{
				if( !csArg.Compare("true"))
					bDetailLog = TRUE;
				else
					bDetailLog = FALSE;
				csResponse = "OK\r\n";
				return;
			}
		}
	}
	csResponse = "Error : Unkown Command.\r\n";
}

int CTerminalAck::initValue()
{
	bInputPasswd = FALSE;
	iTerminalState = 0;
	iLeft = 0;
	iRight = 0;
	bDetailLog = FALSE; // IVR의 상세 로그를 볼 것인지를 결정.
	bCallLog = FALSE;
	bDblEnterDlog = FALSE;
	bDblEnterClog = FALSE;
	bUserID = FALSE;
	bUserPw = FALSE;
	iCommanBuffer = 0;
	iRetrialCount = 0;
	memset( szUserID, 0, 32);
	memset( szUserPW, 0, 32);
	memset( sReceiveBuf, 0, 1024);
	for( int i = 0; i < 10; i++)
	{
		memset( command_history[i], 0, 1024);
	}
	return 0;
}

void CTerminalAck::CommandServiceParse(char *sbuffer)
{
	char * sArg = strstr(sbuffer, " ");
	if( sArg )
	{
		sArg++;
	}
	
	if(!memcmp(sbuffer, "ftp", 3))
	{
		if(!memcmp(sArg,"true",4) || !memcmp(sArg,"TRUE",4))
		{
			Send("OK\r\n", 25);
		}
		else if(!memcmp(sArg,"false",5) || !memcmp(sArg,"FALSE",5))
		{
			Send("OK\r\n", 26);
		}
		else
		{
			Send(ERROR_INVALID_ARG, strlen(ERROR_INVALID_ARG));
		}
	}
}

void CTerminalAck::ViewScenarioFileList(CString &csResponse)
{
	BOOL bFind;
	CFileFind pFF;
	CString csbuf;
	CString csFile;
	bFind = pFF.FindFile("*.scm");
	if( bFind == FALSE)
	{
		csResponse = ERROR_NOT_FOUND_SCE;
		return;
	}
	while( bFind)
	{
		bFind = pFF.FindNextFile();
		csFile = pFF.GetFileTitle();
		csbuf.Format("%s\r\n", csFile);
		Send(csbuf, csbuf.GetLength(), 0);
	}
}

void CTerminalAck::ViewScenarioFile(CString csArg)
{
	FILE * fp;
	char sbuf[256];
	CString csbuf;
	char * pNULL = (char*)strstr( csArg, " ");
	csArg = pNULL;
	csArg.Remove(' ');
	csArg.Remove('\r');
	csArg.Remove('\n');
	csArg +=".scm";
	LogWriteTerminalOperate(csArg.GetBuffer(csArg.GetLength()), "[view scenario]",0, 0);
	if( pNULL++ )
	{
		if( pNULL )
		{
			fp = fopen(csArg, "rt");
			if( fp )
			{
				memset( sbuf, 0, 256);
				while( fgets(sbuf, 256, fp) )
				{
					csbuf = sbuf;
					csbuf.Replace("\n", "\r\n");
					Send(csbuf, csbuf.GetLength());
					memset( sbuf, 0, 256);
				}
				fclose(fp);
				return;
			}
			Send(ERROR_NOT_FOUND_SCE, strlen(ERROR_NOT_FOUND_SCE));
			return;
		}
	}
}

BOOL CTerminalAck::bVerifyUser()
{
	CString csbuf;
	memcpy(szUserID,sReceiveBuf, strlen(sReceiveBuf));
	csbuf = szUserID;
	if( csbuf.GetLength()<= 0)
		return FALSE;
	if( GetUserInfo())
		return TRUE;
	if(!csbuf.Compare(ID_ADMIN_DEFAULT))
		return TRUE;
	return FALSE;
}

BOOL CTerminalAck::bVerifyPasswd()
{
	CString csbuf;
	csbuf =	sReceiveBuf;
	//csbuf = 
	int iLen = strlen( szUserPW);
	if( iLen <= 0)
	{
		memcpy(szUserPW, PW_ADMIN_DEFAULT, strlen(PW_ADMIN_DEFAULT));
	}
	if( csbuf.GetLength()<= 0)
		return FALSE;
	if(!csbuf.Compare(szUserPW))
	{
		memset(szUserPW, 0, 32);
		return TRUE;
	}
	memset(szUserPW, 0, 32);
	return FALSE;
}

void CTerminalAck::AdminUser(int iSubCommand, CString csCommand, CString csResponse)
{
	char * userid = (char*)strstr(csCommand, " ");
	CString csUserID = userid;
	csUserID.Remove((TCHAR)0x20);
	switch(iSubCommand){
	case 1: // 암호 변경
		ModifyPassword(csUserID);
		break;
	case 2: // 사용자 추가
		break;
	}
}

void CTerminalAck::ModifyPassword(CString csUserID)
{
	if( csUserID.Compare(ID_ADMIN_DEFAULT))
	{
		csUserID.Format(USER_ERROR, csUserID);
		Send(csUserID, strlen(csUserID));
		return;
	}
	Send(PW_CHANGE_NOTIFY, strlen(PW_CHANGE_NOTIFY));
	Send(INPUT_NEW_PWD1, strlen(INPUT_NEW_PWD1));
	bInputPasswd = TRUE;
	memset(szUserID, 0, 32);
	memset(sReceiveBuf, 0, 32);
	sprintf(szUserID, "%s", csUserID);
	iRetrialCount = 0;
}

void CTerminalAck::ModifyPassword()
{
	int iLen = strlen(sReceiveBuf);
	if( strlen( szUserPW) <= 0)
	{
		if( iLen > 32)
		{
			memset(sReceiveBuf, 0, 1024);
			Send(TOOLONG_PWD, strlen(TOOLONG_PWD));
			Send(INPUT_NEW_PWD1, strlen(INPUT_NEW_PWD1));
			if( iRetrialCount++ == 3){
				Send(RETRIAL_OVER, strlen(RETRIAL_OVER));
				Send("\n\r>", 3);
				bInputPasswd = FALSE;
				iRetrialCount = 0;
				return;
			}
			return;
		}
		memcpy(szUserPW, sReceiveBuf, iLen);
		Send(INPUT_NEW_PWD2, strlen(INPUT_NEW_PWD2));
		memset(sReceiveBuf, 0, 1024);
		return;
	}
	if( memcmp(szUserPW, sReceiveBuf, iLen))
	{
		memset(szUserPW, 0, 32);
		memset(sReceiveBuf, 0, 1024);
		if( iRetrialCount++ == 3)
		{
			Send(RETRIAL_OVER, strlen(RETRIAL_OVER));
			Send("\n\r>", 3);
			bInputPasswd = FALSE;
			iRetrialCount = 0;
			return;
		}
		Send(NOT_MATCH, strlen(NOT_MATCH));
		Send(INPUT_NEW_PWD1, strlen(INPUT_NEW_PWD1));
		return;
	}
	else
	{
		memset(sReceiveBuf, 0, 1024);
		bInputPasswd = FALSE;
		WriteIDnPasswd();
		memset(szUserPW, 0, 32);
		Send(MODIFY_SUCCESS, strlen(MODIFY_SUCCESS));
		Send("\n\r>", 3);
	}
}

//암호를 저장한다.
void CTerminalAck::WriteIDnPasswd()
{
	int iCount = 0;
	int iTotalCount = 0;
	iTotalCount = iFindUserID(iCount);
	if( iCount < 0)
	{
		iCount = iTotalCount;
	}
	CString csUserAccount;
	CString csData = szUserID;
	csData += szUserPW;
	csUserAccount.Format("User%d", iCount);
	AfxGetApp()->WriteProfileString("BIS_IVR", csUserAccount, csData);
}

//return Value : 전체 등록된 사용자 계정의 갯 수
//Argument : OUT int &iCount - 발견된 사용자의 번호.
int CTerminalAck::iFindUserID(OUT int &iCount)
{
	CString csUserAccount;
	int iSum = 0;
	iCount = 0;
	
	csUserAccount.Format("User%d", iCount);
	CString csUser =AfxGetApp()->GetProfileString("BIS_IVR", csUserAccount, "none");
	
	while( csUser.Compare("none"))
	{
		iSum ++;
		int iLen = strlen(szUserID);
		if(!memcmp(szUserID, csUser, iLen)) return iCount;
		else iCount++;
	}
	iCount = -1;
	return iSum;
}

// 사용자의 ID와 패스워드를 갖고 온다.
//사용자의 ID가 존재하면 TRUE를 리턴하고,
//패스워드를 szUserPW에 복사한다.
//재 검 필요. 2003-04-23-18-05
BOOL CTerminalAck::GetUserInfo()
{
	int iCount = 0;
	iFindUserID(iCount);
	if( iCount < 0)
		return FALSE;
	CString csUserAccount;
	csUserAccount.Format("User%d", iCount);
	CString csUser =AfxGetApp()->GetProfileString("BIS_IVR", csUserAccount, "none");
	int iLen = strlen(szUserID);
	memset(szUserPW, 0, 32);
	csUser = csUser.Right(csUser.GetLength()-iLen);
	sprintf(szUserPW, "%s", csUser);
	return TRUE;
}