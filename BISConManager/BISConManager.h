// BISConManager.h : main header file for the BISCONMANAGER application
//

#if !defined(AFX_BISCONMANAGER_H__4D256E02_18B6_4CF0_AFE8_43E8E2E9C348__INCLUDED_)
#define AFX_BISCONMANAGER_H__4D256E02_18B6_4CF0_AFE8_43E8E2E9C348__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//#include "ClientSock.h"
#include "IvrParsor.h"
#include "CallInfo.h"

#include "Global.h"

class CFtpServer;
class CTerminal;
class CStateView;
class CMainFrame;

/////////////////////////////////////////////////////////////////////////////
// CBISConManagerApp:
// See BISConManager.cpp for the implementation of this class
//

class CBISConManagerApp : public CWinApp
{
public:
	void CloseTerminal();
	BOOL bInitGlobalMessageHandle();
	void StopTerminalService();
	void StartTerminalService();
	BOOL bInitGlobalMemory();
	BOOL InitGlobal();
	CBISConManagerApp();
	~CBISConManagerApp();

	void				ConfDoModal();
	void				SetToolBar(CCmdUI *pCmdUI);
	void				IVRMSGProc(UINT message, WPARAM wParam, LPARAM lParam);
	void				AcroIvrInterProcRes(WPARAM wParam, LPARAM lParam);
	BOOL				bReleaseGlobalMemory();

	void				SendRegMessage();
	BOOL				bFindIvrProc();
	BOOL				bRunIvrProcess();
	void				SetCallInfo();
	void				SetVariable();
	void				SetIVRStart();
	void				SetIVRStop();
	void				ParsePacket(char *packet);
	void				OnVoiconOp();
	void				OnTerminal();

	BOOL				bStartTerminal;	//터미널 서비스가 시작되었는지를 나타내는 플러그.
	BOOL				bIvrStart;
	
	BOOL				bForceEnded;
	HANDLE				hIvrHandle;
	BOOL				bForceDrop;
	HANDLE				hKeepAlive;
	BOOL				bSetTimer;// 프로세스 감시 타이머가 작동중인지를 갖는 플래그.
	BOOL				bAutoStart; // up 명령어 후에 바로 시작할 것인지를 결정.
	
	CTerminal			*terminal;
	CFtpServer			*ftp;
	//
	//터미널 처리를 위해서 사용되어짐
	//
	// 메모리 핸들
	HANDLE				hSharedMem1;		// 콜의 정보를 전달하기 위한 버퍼.
	// 버퍼의 참조 포인트
	char				*sSharedMemory1;	// 콜의 정보를 전달하는 버퍼의 참조 포인터
	
	int					iCallNum;
	
	BOOL				bEnterInputStateVOICONStart;
	//HWND				partner_hWnd;		//
	UINT				MessageState;		// 글로벌 메세지 핸들 ; Send State
	UINT				MessageReq;			// 글로벌 메세지 핸들 : Request message
	UINT				MessageRes;			// 글로벌 메세지 핸들 : response message
	UINT				MessageReg;			// 글로벌 메세지 핸들 : message for regist each other.
	UINT				MessageRek;			// 글로벌 메세지 핸들 : message for regist each other.
	UINT				RECVDETAILLOG;		// 로그 전달 핸들
	UINT				RECVCALLINFO;		// message
	UINT				MessageShow;		// 글로벌 메세지 핸들 
	
	//CRITICAL_SECTION	CriticalLogWite;
	
	//HANDLE				hAcroIvrReadLog;	// 상세 로그를 터미널이 읽었는지 여부를 알 수 있는 이벤트 핸들러

	HANDLE				hMutex;				// 2중 실행 방지 
	// IVR 프로세서의 정보
	PROCESS_INFORMATION	pProcessInformation;

	// end : by swhors
	CStateView			*m_pStateView;
	CMainFrame			*m_pMainFrame;
	
	IVR_SYSTEM			m_pIVR_System;
	CIVRParsor			*m_pParsor;
	CCallInfo			*m_pCallInfo;
	int					m_nCallCount;
	int					m_nIvrIsRun;			// ivr is run tag
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBISConManagerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CBISConManagerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnTmStart();
	afx_msg void OnTmStop();
	afx_msg void OnVoiconStart();
	afx_msg void OnVoiconStop();
	afx_msg void OnConfig();
	afx_msg void OnUpdateTmStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTmStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVoiconStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVoiconStop(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BISCONMANAGER_H__4D256E02_18B6_4CF0_AFE8_43E8E2E9C348__INCLUDED_)
