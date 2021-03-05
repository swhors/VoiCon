// IVRRunner.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include "shellapi.h"

//rad include
#include <cm.h>
#include "operate.h"
#include "ivrkeydata.h"
#include "management.h"

#include "msg_define.h"

#include "plugindef.h"
#include "scm.h"

#include "voice.h"
#include "plugin.h"
#include "rtp.h"
#include "rtcp.h"
#include "rtptest.h"
#include "scm.h"
#include "operate_ment.h"
#include "operate_scenario.h"
#include "radmiscfunction.h"

//#include "ExceptionHandler.h"

#include "sky_rtp.h"

#define MAX_LOADSTRING 100

#define TID_SEND_REG_STATE		10101
#define TVAL_SEND_REG_STATE		400

#define MESSAGE_CMD_TRAY	"IVRTRAYCOMMANDMSG"


HANDLE hMutex;

void CALLBACK LogManager(HWND m_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime);

UINT MESSAGECMDTRAY;

#define WPA_CALL_NUM	0x01
#define WPA_USER_NUM	0x02

//int		logCount;

#pragma pack (push, CMD_ALINMENT, 1)

// Global Variables:
HINSTANCE			hInst;							// current instance
HWND				m_hWnd;
TCHAR				szTitle[MAX_LOADSTRING];		// The title bar text
TCHAR				szWindowClass[MAX_LOADSTRING];	// The title bar text

// IVR 프로그램의 메인 시작 위치;
char				szIvrMainPath[256];

//
//터미널 처리를 위해서 사용되어짐
//
// 터미널과 IVR간의 정보 전달 버퍼의 핸들
HANDLE				hSharedMem1;					// 콜의 정보를 전달하기 위한 버퍼.
// 버퍼의 참조 포인트
char				*sSharedMemory1;				// 콜의 정보를 전달하는 버퍼의 참조 포인터

//원격의 컨트롤러와 통신을 하기 위한 메세지 핸들러
UINT				MessageState;	// 글로벌 메세지 핸들 : State Message ( registered state )
UINT				MessageReq;		// 글로벌 메세지 핸들 : Request message
UINT				MessageRes;		// 글로벌 메세지 핸들 : response message
UINT				MessageReg;		// 글로벌 메세지 핸들 : message for regist each other.
UINT				MessageRek;		// 글로벌 메세지 핸들 : message for regist each other.
UINT				RECVCALLINFO;	// message
UINT				MessageShow;	// 글로벌 메세지 핸들 

char *sCodec[MAX_SUPP_CODEC]={"g7231", "g711a", "g711u","g729", "g729a"};

// Ivr default information
int					iMaxAllowedCall;
BOOL				bEnableCallCoupl;
int					iInstalledLang;
TCHAR				sLanguage[8][4];
//int					iIvrDefaultSilenceSupression;
//BOOL				bAutoRun;

// 음성 파일과 관련된 변수
PVOICE_PAGE			pVoicePage; //음성 파일의 페이지
								//음성 파일의 하위폴더별로 정보를 가짐.
int					iVocePageCount; // 음성 파일 페이지의 수

// Plugin Information.
HANDLE_BUFFER		handleBuffer[10];
int					iDllCount;

// Scenario Information
int					sf_count;				// scenario file count;
int					iInputWaitTime;
int					iInputInitWaitTime;
SCM					*scm;

// H323 Information
TCHAR				szH323ID[32];
TCHAR				szE164ID[32];
TCHAR				szGlobalGkIp[24];
int					iGkPort;
int					iMaxDelayJitter;
int					iTTL;
HAPP				hApp;
int					m_iFastStart;
int					m_iH245Tunneling;

// Call Information
CONNECTION			pCon[400];

// License Information
BOOL				bIsUSBEnable;
IVRFILELOCK			keyData;

BOOL				bRunState;				// IVR이 런 상태를 가짐.
BOOL				bTerminate;

NOTIFYICONDATA		m_tnd;
HICON				hTrayIcon;
UINT				iTotalCallNum;

// special codec support
int					m_g729Support;

// rtp Port
int					m_InitOfRtpPort;

HANDLE handleOfManualRegThread;

//CRITICAL_SECTION	hAPP_CRITICAL;

#pragma pack (pop, CMD_ALINMENT)

void WriteLog(char * filename, int a, char * c, char * b);

BOOL				m_bRegisteredBISCon;
#define				REGISTEREDCHK_TIMERID	20030
#define				REGISTEREDCHK_TIMEVAL	8000

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// URQ를 받고 다시 등록하기 위한 콜백 함수
void CALLBACK RERegister(HWND m_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
	KillTimer(m_hWnd, 29899);
	if( hApp ) cmRegister(hApp);
	WriteLogL(PROG_NAME, -2, "URR", "Event WM_BISCON_REREGISTER.");
}

void CALLBACK TimerSendMaxCall(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
	KillTimer(l_hWnd, eEventId);//ACRO_IVR_REGISTERED_FLAG
	SendMessage(HWND_BROADCAST, MessageReq, ACRO_IVR_MAXCALL_SEND, (LPARAM)iTotalCallNum);
	SendMessage(HWND_BROADCAST, MessageReq, ACRO_IVR_REGISTERED_FLAG, (LPARAM)m_bRegisteredBISCon);
	SetTimer(l_hWnd, eEventId, 200, TimerSendMaxCall);
}

void CALLBACK TimerSendCallState(HWND hWnd,UINT uMsg,UINT eId,DWORD	dwTime)
{
	KillTimer(hWnd, eId);
	SendMessage(HWND_BROADCAST, MessageState, NULL, (LPARAM) m_bRegisteredBISCon);
	SetTimer(hWnd, eId,TVAL_SEND_REG_STATE,TimerSendCallState);
}

DWORD WINAPI RunManualRegistration(PVOID pArg)
{
	HANDLE hEvent;
	int iCount;
	int iFlip=0;
	int iTTLValue = GetPrivateProfileInt("BISCON_RAS", "RAS_TimeToLive", 120000, "./biscon.ini");
	if( iTTLValue < 10000 && iTTLValue > 480000)
		iTTLValue = 119800;
	else
		iTTLValue = iTTLValue - 1000;
	hEvent = CreateEvent(NULL, FALSE, FALSE, "ManualRegistrationThread");
	if( hEvent )
	{
		iCount = iTTLValue/100;
		while(!bRunState )
		{
			WaitForSingleObject(hEvent, 150);
		}
		while(bRunState)
		{
			// 119800
			WriteLogL(PROG_NAME, -1, "RRQ", "[TIMER EVENT] Send RRQ.");
			for(int i=0; i < iCount;i++)
			{
				if( !bTerminate )
					WaitForSingleObject(hEvent, 100);
				else
					break;
				if( (iFlip==4) && !m_bRegisteredBISCon)
				{
					if( hApp )
						cmRegister(hApp);
					iFlip = 0;
				}
				else
					iFlip++;
			}
			if( hApp )
				cmRegister(hApp);
		}
		CloseHandle(hEvent);
	}

	if( handleOfManualRegThread )
		CloseHandle(handleOfManualRegThread);
	WriteLogL(PROG_NAME, -1, "RRQ", "[TIMER EVENT] Close.");
	return 1;
}

// 처음 등록 후 등록이 되지 않을 경우, 임으로 재 등록 하도록 함.
void CALLBACK m_bRegisteredBISConChk(HWND l_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
	KillTimer(m_hWnd, eEventId);
	if( eEventId == REGISTEREDCHK_TIMERID )
	{
		if( bRunState )
		{
			if( !m_bRegisteredBISCon )
			{
				if( hApp )
					cmRegister(hApp);
				WriteLogL(PROG_NAME, -1, "REG", "Not Registered-ReEnter.");
			}
			else
				WriteLogL(PROG_NAME, -1, "REG", "Registered.");
			SetTimer(l_hWnd, eEventId, REGISTEREDCHK_TIMEVAL, m_bRegisteredBISConChk);
		}
	}
}

void CALLBACK IncorrectCallCheck(HWND thWnd,UINT uMsg,UINT eVentID, DWORD dwTime)
{
	int i =0;
	KillTimer(thWnd, eVentID);
	for( i=0; i< iMaxAllowedCall;i++)
	{
		if( pCon[i].bCallReusing && !pCon[i].bOpendChannel &&  !pCon[i].bUsedThisCall )
		{
			if( !pCon[i+iMaxAllowedCall].bCallReusing || pCon[i+iMaxAllowedCall].bOpendChannel 
				|| pCon[i+iMaxAllowedCall].bUsedThisCall )
			{
				WriteLog("ivrop", i, "0", "IncorrectCallCheck");
				SendMessage(thWnd, WM_VOICON_CallDrop, i+iMaxAllowedCall, cmReasonTypeUndefinedReason);
			}
			else if( scm[i].m_bCon)
			{
				WriteLog("ivrop", i, "1", "IncorrectCallCheck");
				TerminateThread(pCon[i].thread_sce, 0);
				CloseHandle(pCon[i].thread_sce);
				pCon[i].thread_sce = NULL;
				scm[i].m_bCon = FALSE;
			}
		}
		else
		{
			if( pCon[i].bCallReusing )
			{
				if( pCon[i].hsCall )
					cmCallDrop(pCon[i].hsCall);
			}
		}
	}
	SetTimer(thWnd, 17171, 60000,  IncorrectCallCheck);
}


VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt)
{ 
    HMENU hmenu;            // top-level menu 
    HMENU hmenuTrackPopup;  // shortcut menu 
	
    // Load the menu resource. 
	
    if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_TRAY_MENU))) == NULL) 
        return;
	
    // TrackPopupMenu cannot display the menu bar so get
    // a handle to the first shortcut menu.
	
    hmenuTrackPopup = GetSubMenu(hmenu, 0);
	
    // Display the shortcut menu. Track the right mouse
    // button.
	
    TrackPopupMenu(hmenuTrackPopup,
		TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, 0, hwnd, NULL);
    // Destroy the menu.
	
    DestroyMenu(hmenu);
}

UINT AddSystemTray(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
{
	BOOL res;  
	
    m_tnd.cbSize = sizeof(NOTIFYICONDATA); 
    m_tnd.hWnd = hwnd; 
    m_tnd.uID = uID; 
    m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    m_tnd.uCallbackMessage = WM_NOTIFICATION; 
    m_tnd.hIcon = hicon; 
    if (lpszTip) 
        lstrcpyn(m_tnd.szTip, lpszTip, sizeof(m_tnd.szTip)); 
    else 
        m_tnd.szTip[0] = '\0'; 
	
    res = Shell_NotifyIcon(NIM_ADD, &m_tnd);
	
    if (hicon) 
        DestroyIcon(hicon); 
	
    return 0; 
}

LRESULT CALLBACK TrayDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hItemHwnd;
	switch (uMsg)
	{
	case WM_DESTROY:
		KillTimer(hDlg, 20000);
		break;
	case WM_INITDIALOG:
		{
			if( !bRunState )
				hItemHwnd = GetDlgItem(hDlg, IDC_STOP);
			else
				hItemHwnd = GetDlgItem(hDlg, IDC_RUN);
			EnableWindow(hItemHwnd, FALSE);
			SetTimer(hDlg, 20000, 10, NULL);
		}
		return TRUE;
	case WM_TIMER:
		{
			switch( LOWORD(wParam))
			{
			case 20000:
				{
					char cNum[24];
					KillTimer(hDlg, 20000);
					sprintf(cNum, "%d", iMaxAllowedCall);
					SetDlgItemText(hDlg, IDC_ST_CALL, cNum);
					SetTimer(hDlg, 20000, 500, NULL);
				}
				break;
			default:
				break;
			}
		}
		break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam))
			{
			case IDOK:
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			case IDC_RUN:
				{
					if( hApp && !bRunState)
					{
						cmStart(hApp);
						// by swhors 2007 03 22 0145
						cmRegister(hApp);
						start_rtp_manager(iMaxAllowedCall, m_InitOfRtpPort);
						//SetTimer(m_hWnd, REGISTEREDCHK_TIMERID, REGISTEREDCHK_TIMEVAL,m_bRegisteredBISConChk);
						bRunState = TRUE;
					}
					else
						MessageBox(NULL, "Already ended..", "error", MB_OK);
				}
				EndDialog(hDlg, LOWORD(wParam));
				break;
			case IDC_STOP:
				{
					if( hApp && bRunState)
					{
						bRunState = FALSE;
						cmStop(hApp);
						KillTimer(m_hWnd, REGISTEREDCHK_TIMERID);
						KillTimer(m_hWnd, TID_SEND_REG_STATE);
//#define 		10101
//#define TVAL_SEND_REG_STATE		1000

						//cmRegister(hApp);
					}
					else
					{
						MessageBox(NULL, "Already started..", "error", MB_OK);
					}
				}
				EndDialog(hDlg, LOWORD(wParam));
				break;
				break;
			}
			break;
		}
	default:
		{
			if( uMsg == MESSAGECMDTRAY )
			{
				switch(LOWORD(wParam))
				{
				case WPA_CALL_NUM:
					break;
				case WPA_USER_NUM:
					{
						char uNum[24];
						sprintf(uNum, "%d", lParam);
						SetDlgItemText(hDlg, IDC_ST_USER, uNum);
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	}
    return FALSE;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// block Double access 
	hMutex = CreateMutex(NULL, FALSE, "BISCONRUNNER");
	if (GetLastError() == ERROR_ALREADY_EXISTS) 
    {
        return FALSE;
    }
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IVRRUNNER, szWindowClass, MAX_LOADSTRING);
	hTrayIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IVRRUNNER));
	
	MyRegisterClass(hInstance);
	bRunState = FALSE;
	//iIvrDefaultSilenceSupression = 0;
	//InitLogVar();
	WriteLogL(PROG_NAME, -1, "IVR", "Start Aplication");
	// Perform application initialization:
	if (!InitInstance (hInstance, SW_HIDE)) 
	{
		DeleteLogVar();
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IVRRUNNER);
	
	AddSystemTray(m_hWnd, IDD_TRAY_DLG, hTrayIcon, "BISConRunner");
	MESSAGECMDTRAY = 0;
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return msg.wParam;
}


void SendMessageOther(UINT MSG, WPARAM wParam, LPARAM lParam)
{
	// 아래의 라인은 partber_hWnd의 부정확성으로 HWND_BROADCAST로 
	// 단일화하여 사용함.
	// 2006/06/12 02:15 swhors@naver.com
//	if( partner_hWnd )
//		::SendMessage(partner_hWnd, MSG, wParam, lParam);
//	else
	::SendMessage(HWND_BROADCAST, MSG, wParam, lParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IVRRUNNER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IVRRUNNER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	
	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	
	m_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (!m_hWnd)
	{
		return FALSE;
	}
	
	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);
	
	return TRUE;
}

void AcroIvrInterProcReq(WPARAM wParam, LPARAM lParam)
{
	switch( wParam)
	{
	case ACRO_IVR_SCENARIO_UNLOAD:
		{

		}
		break;
	case ACRO_IVR_SCENARIO_LOAD:
		{
		}
		break;
	case ACRO_IVR_RESETCALL:
		{
			// 메세지 정의 삽입 .
			// 기능 추가.
			// 원격에서 호를 끊게 함.
			int iCallNum = (int)lParam;
			int iOtherNum = iCallNum + iMaxAllowedCall;
			//
			if( !pCon[iCallNum].bCallReusing || pCon[iCallNum].bOpendChannel
				|| pCon[iCallNum].bUsedThisCall || !scm[iCallNum].m_bCon
				|| !pCon[iOtherNum].bCallReusing || pCon[iOtherNum].bOpendChannel
				|| pCon[iOtherNum].bUsedThisCall)
			{
				// 강제로 종료하거나, 잘못된 콜을 강제 해제 할 경우, 
				// 이 루틴을 타게 됨.
				if( !pCon[iCallNum].bCallReusing || pCon[iCallNum].bOpendChannel ||
					pCon[iCallNum].bUsedThisCall)
				{
					WriteLog("ivrop", iCallNum, "ACRO_IVR_RESETCALL", "0");
					//인바운드 정보가 살아 있음.
					//호를 강제 해제함.
					DropCall( iCallNum, cmReasonTypeUndefinedReason);
				}
				else if(!pCon[iOtherNum].bCallReusing || pCon[iOtherNum].bOpendChannel ||
					pCon[iOtherNum].bUsedThisCall )
				{
					WriteLog("ivrop", iCallNum, "ACRO_IVR_RESETCALL", "1");
					// 인바운드 호는 죽었으나, 인바운드 호가 죽었음.
					DropCall( iOtherNum, cmReasonTypeUndefinedReason);
				}
				else if( scm[iCallNum].m_bCon)
				{
					WriteLog("ivrop", iCallNum, "ACRO_IVR_RESETCALL", "2");
					// 인바운드와 아웃 바인드 호 모두가 죽었으나, 시나리오에서 
					// 빠져 나가지 못했음.
					// 시나리오 종료.
					// 현재로서는 ??
					// 2006 / 8 / 8 swhors@naver.com
					TerminateThread(pCon[iCallNum].thread_sce,0);
					if( pCon[iCallNum].thread_sce )
						CloseHandle(pCon[iCallNum].thread_sce);
					pCon[iCallNum].thread_sce = NULL;
					scm[iCallNum].m_bCon = FALSE;
				}
				else
				{
					WriteLog("ivrop", iCallNum, "ACRO_IVR_RESETCALL", "3");
				}
			}
			break;
		}
	case ACRO_IVR_GETRUNSTATE_REQ:
		WriteLog("ivrop", NULL, 0, "ACRO_IVR_GETRUNSTATE_REQ");
		SendMessageOther(MessageRes, ACRO_IVR_GETRUNSTATE_ACK, (long)bRunState);
		break;
	case ACRO_IVR_GETCONSTATE_REQ:
		WriteLog("ivrop", NULL, 0, "ACRO_IVR_GETCONSTATE_REQ");
		SendMessageOther(MessageRes, ACRO_IVR_GETCONSTATE_ACK, 0);
		break;
	case ACRO_IVR_RUN_REQ:
		if( !bRunState )
		{

		}
		WriteLog("ivrop", NULL, 0, "ACRO_IVR_RUN_REQ");
		SendMessageOther(MessageRes, ACRO_IVR_RUN_ACK, (long)bRunState);
		break;
	case ACRO_IVR_STOP_REQ:
		WriteLog("ivrop", NULL, 0, "IVR_STOP_REQ");

		SendMessageOther(MessageRes, ACRO_IVR_STOP_ACK, (long)bRunState);
		break;
	case ACRO_IVR_DOWN_REQ:
		{
			if( bRunState )
			{
			}
			WriteLog("ivrop", NULL, 0, "IVR_DOWN_REQ");
			SendMessageOther(MessageRes, ACRO_IVR_DOWN_ACK, 0);
			//partner_hWnd = NULL;
		}
		break;
	case ACRO_IVR_RES_CL_REQ:
		{
			if( bRunState )
			{
				bRunState = FALSE;
				bTerminate = TRUE;
				for( int i = 0; i < iMaxAllowedCall; i++)
				{
					if( pCon[i].bUsedThisCall )
					{
						DropCall( i, cmReasonTypeUndefinedReason);
					}
				}
				HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, "ACRO_IVR_RES_CL_REQ");
				if( hEvent )
				{
					WaitForSingleObject(hEvent, 2000);
					CloseHandle(hEvent);
				}
				else
					Sleep(2000);
			}
			WriteLog("ivrop", NULL, 0, "IVR_RES_CL_REQ");
			// Previous Code
			// removed by swhors@naver.com 2006/06/22 16:57
			// start ---------------
			//EndOfOperate();
			//Sleep(100);
			//bReleaseGlobalMemory();
			//Shell_NotifyIcon(NIM_DELETE, &m_tnd);
			// end   ---------------
			
			// append below line : swhors@naver.com 2006/06//2 16:17
			SendMessage(m_hWnd, WM_DESTROY, NULL, NULL);
			
			SendMessageOther(MessageRes, ACRO_IVR_RES_CL_ACK, (long)bRunState);
			
			// removed below line : swhors@naver.com 2006/06//2 16:17
			//PostQuitMessage(0);
		}
		break;
	}
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	
	char textbuff[1024];
	memset(textbuff, 0 , 1024);
	
	switch (message)
	{
	case WM_CREATE:
		//case WM_INITDIALOG:
		//partner_hWnd = HWND_BROADCAST;
		m_hWnd = hWnd;
		handleOfManualRegThread = NULL;
		MESSAGECMDTRAY = RegisterWindowMessage(MESSAGE_CMD_TRAY);
		memset(szIvrMainPath, 0, 256);
		GetCurrentDirectory( 256,szIvrMainPath);
		WriteLogL(PROG_NAME, -1, "OOP", szIvrMainPath);
		bTerminate = FALSE;
		m_g729Support = 0;
		m_InitOfRtpPort =0;
		//hHeap = NULL;
		SetTimer(hWnd, 21020, 200, TimerSendMaxCall);
		// 2006/08/07 16:27 swhors@naver.com
		// IVR의 등록에 대한 플래그.
		iTotalCallNum = 0;
		m_bRegisteredBISCon	= 0;
		m_iFastStart = 0;
		m_iH245Tunneling = 0;
		iDllCount = 0;
		WriteLog("TELOG", -1, "WM_CREATE", "3");
		
		// 에러 발생시, 에러 메세지 박스가 보이지 않게 함.
		SetErrorMode(SEM_NOGPFAULTERRORBOX);
		
		if( registerGlobalMessage())
		{
			if( bInitGlobalMemory() )
			{
				SendMessageOther(MessageReg, (LONG)hWnd, NULL);
				WriteLog("TELOG", -1, "WM_CREATE", "2");
				if( ParseConfigure(hWnd))
				{
					//InitializeCriticalSection(&hFreeCritical);
					//InitializeCriticalSection(&hMakeCritical);
					//if( bAutoRun )
					{
						WriteLog("TELOG", -1, "WM_CREATE", "1");
						if( hApp)
						{
							cmStart(hApp);
							// by swhors 2007 03 22 0145
							//if( !GetPrivateProfileInt("BISCON_RAS", "RAS_ManualRegistration", 0, "./biscon.ini") )
								cmRegister(hApp);
							start_rtp_manager(iMaxAllowedCall, m_InitOfRtpPort);
							bRunState = TRUE;
							WriteLog("ivrop", 0, NULL, "IVR_START[WM_CREATE]");
							SetTimer(hWnd, TID_SEND_REG_STATE,TVAL_SEND_REG_STATE,TimerSendCallState);
							SetTimer(hWnd, LOG_TTL_TID, LOG_TTL_TVAL, LogManager);
							WriteLog("TELOG", -1, "WM_CREATE", "0");
							//SetTimer(hWnd, REGISTEREDCHK_TIMERID, REGISTEREDCHK_TIMEVAL,m_bRegisteredBISConChk);
						}
						else
						{
							WriteLog("ivrop", 0, "HAPP Error", "IVR_START[WM_CREATE]");
							SendMessage(hWnd, WM_DESTROY, NULL, NULL);
							return FALSE;
						}
					}
					//else
					//{
					//	DialogBox(hInst, (LPCTSTR)IDD_TRAY_DLG, hWnd, (DLGPROC)TrayDlgProc);
					//}
					//SetTimer(hWnd, 17171, 60000,  IncorrectCallCheck);
					//updated at 2004 04 30. by wonny.
					
					//////////////////////////////
					//임시로 막음.
					SendMessageOther(MessageRes, ACRO_IVR_UP_COMPLETE, 0);
					//
					//////////////////////////////
					//LoadExceptionFilter();
					return TRUE;
				}
			}
			else
			{
				MessageBox(NULL,"Can't initialize global handler.", "error", MB_OK);
			}
		}
		SendMessage(hWnd, WM_DESTROY, 0, 0);
		return FALSE;
	case WM_BISCON_MANUALREGIST:
#if 0
		SetTimer(hWnd, RAD_MANUALREGISTRATION_TID, RAD_MANUALREGISTRATION_TVAL, NULL);
		WriteLogL(PROG_NAME, -1, "RRQ", "[MESSAGE EVENT] Start RRQ Timer.");
#else
		DWORD dwId;
		handleOfManualRegThread = CreateThread(
				NULL, // no security attributes 
				0, // use default stack size  
				RunManualRegistration, // thread function 
				NULL, // argument to thread function 
				0, // use default creation flags  
				&dwId); // returns the thread identifier 		CreateThread(
#endif
		break;
	case WM_TIMER:
		{
			{
				char tbuf[64];
				wsprintf(tbuf, "RECV_TIMER : %ud_%ud", wParam, LOWORD(wParam));
				WriteLogL(PROG_NAME, -1, "OOP", tbuf);
			}
			switch( LOWORD(wParam) )
			{
			case RAD_MANUALREGISTRATION_TID:
				{
					//KillTimer(hWnd, RAD_MANUALREGISTRATION_TID);
					if( hApp )
						cmRegister(hApp);
					//SetTimer(hWnd, RAD_MANUALREGISTRATION_TID, RAD_MANUALREGISTRATION_TVAL, NULL);
					WriteLogL(PROG_NAME, -1, "RRQ", "[TIMER EVENT] Send RRQ.");
				}
				break;
			default:
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		break;
	case WM_NOTIFICATION:
		{
			if( LOWORD(lParam) == WM_RBUTTONUP)
			{
				POINT Point;
				if( GetCursorPos( &Point) )
				{
					DisplayContextMenu(hWnd, Point);
				}
			}

			if (LOWORD(lParam) == WM_LBUTTONDBLCLK) 
			{

				::SendMessageOther(MessageShow, (long)m_hWnd, 0);
			}
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_RUN:
			DialogBox(hInst, (LPCTSTR)IDD_TRAY_DLG, hWnd, (DLGPROC)TrayDlgProc);
			//SendMessage(HWND_BROADCAST, MESSAGECMDTRAY, WPA_CALL_NUM, (LPARAM)iMaxAllowedCall);
			break;
		case IDM_ABOUT:
			WriteLog("TELOG", -1, "IDM_ABOUT", NULL);
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		//hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		//RECT rt;
		//GetClientRect(hWnd, &rt);
		//DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
		//EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		KillTimer(hWnd, 17171);
		WriteLog("ivrop", 0, NULL, "IVR_STOP[WM_CLOSE]");
		break;
	case WM_QUIT:
		WriteLog("ivrop", 0, NULL, "IVR_STOP[WM_QUIT]");
		break;
	case WM_DESTROY:
		{
			CloseHandle(hMutex);
			KillTimer(hWnd,TID_SEND_REG_STATE);
			WriteLog("ivrop", 0, NULL, "IVR_STOP[WM_DESTROY]");
			bRunState = FALSE;
			EndOfOperate();
			bTerminate = FALSE;
			HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,"WM_DESTROY");
			if( hEvent )
			{
				WaitForSingleObject(hEvent,100);
				CloseHandle(hEvent);
			}
			else
				Sleep(100);
			bReleaseGlobalMemory();
			//UnLoadExceptionFilter();
			Shell_NotifyIcon(NIM_DELETE, &m_tnd);
			DeleteLogVar();
			PostQuitMessage(0);
		}
		break;
	case WM_BISCON_REREGISTER:
		SetTimer(hWnd, 29899, 700, RERegister);
		break;
	// user defined message
	case WM_VOICON_CallDrop:
		{
			int iCallNum = (int) wParam;
			cmReasonType DropReason = (cmReasonType )lParam;
			DropCall( iCallNum, DropReason);
		}
		break;
	case WM_VOICON_OutCallCONNET:
		{
			int iCallNum = (int) wParam;
			WriteLogL(PROG_NAME, iCallNum, "IVR", "iCallNumber-outconnect");
			//OutputCallConnect( iCallNum );
		}
		break;

	case WM_VOICON_CallMake:
		{
			//EnterCriticalSection(&hAPP_CRITICAL);
			int iCallNum = (int) wParam;
			MakeCall(scm[iCallNum].billingPacket.pBody.Destination, 
				pCon[iCallNum].src_e164, iCallNum);
			//LeaveCriticalSection(&hAPP_CRITICAL);
		}
		break;
	case WM_VOICON_CallStart:
		{
#if 1
//			WriteLogL(PROG_NAME, 100, "EnterIn", "callstart");
			//EnterCriticalSection(&cri_scm_manager);
			DWORD dwThreadId=0;
			int iCallNum = (int) wParam;
//			WriteLogL(PROG_NAME, iCallNum, "Enter", "callstart");
			pCon[iCallNum].thread_sce = CreateThread(
				NULL, // no security attributes 
				0, // use default stack size  
				ScenarioRunner, // thread function 
				&scm[iCallNum], // argument to thread function 
				0, // use default creation flags  
				&dwThreadId); // returns the thread identifier 
			//LeaveCriticalSection(&cri_scm_manager);
//			WriteLogL(PROG_NAME, iCallNum, "Leave", "callstart");
#endif
		}
		break;
	case WM_VOICON_CallForward:
		{
			int iCallNum = (int) wParam;
			ForwardCall(scm[iCallNum].billingPacket.pBody.Destination, 
				pCon[iCallNum].src_e164, iCallNum);

		}
		break;
	case WM_VOICON_SendDTMF:
		{
			
			//WriteLogL(PROG_NAME, 200, "EnterIn", "sneddtmf");
			//EnterCriticalSection(&hAPP_CRITICAL);

			int iCallNum = (int) wParam;

			cmUserInputData userData;

			if( pCon[(int)wParam + iMaxAllowedCall].hsCall)
			{
				char tbuf[3];
				sprintf(tbuf, "%c", (char)lParam);
				userData.data = tbuf;
				userData.length = strlen( tbuf);
				int id = cmUserInputBuildAlphanumeric( hApp, &userData);
				int ret = cmCallSendUserInput( pCon[(int)wParam + iMaxAllowedCall].hsCall, id);
			}
		}
		break;
	default:
		{
			if( message == MessageReq)
			{
				AcroIvrInterProcReq(wParam, lParam);
				break;
			}
			else if( message == MessageReg || message == MessageRek)
			{
				if( m_hWnd != (HWND) wParam)
				{
					if( message == MessageReg)
					{
						::SendMessageOther(MessageRek, (long)m_hWnd, 0);
						break;
					}
				}
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
		
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

