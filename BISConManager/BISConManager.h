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

	BOOL				bStartTerminal;	//�͹̳� ���񽺰� ���۵Ǿ������� ��Ÿ���� �÷���.
	BOOL				bIvrStart;
	
	BOOL				bForceEnded;
	HANDLE				hIvrHandle;
	BOOL				bForceDrop;
	HANDLE				hKeepAlive;
	BOOL				bSetTimer;// ���μ��� ���� Ÿ�̸Ӱ� �۵��������� ���� �÷���.
	BOOL				bAutoStart; // up ��ɾ� �Ŀ� �ٷ� ������ �������� ����.
	
	CTerminal			*terminal;
	CFtpServer			*ftp;
	//
	//�͹̳� ó���� ���ؼ� ���Ǿ���
	//
	// �޸� �ڵ�
	HANDLE				hSharedMem1;		// ���� ������ �����ϱ� ���� ����.
	// ������ ���� ����Ʈ
	char				*sSharedMemory1;	// ���� ������ �����ϴ� ������ ���� ������
	
	int					iCallNum;
	
	BOOL				bEnterInputStateVOICONStart;
	//HWND				partner_hWnd;		//
	UINT				MessageState;		// �۷ι� �޼��� �ڵ� ; Send State
	UINT				MessageReq;			// �۷ι� �޼��� �ڵ� : Request message
	UINT				MessageRes;			// �۷ι� �޼��� �ڵ� : response message
	UINT				MessageReg;			// �۷ι� �޼��� �ڵ� : message for regist each other.
	UINT				MessageRek;			// �۷ι� �޼��� �ڵ� : message for regist each other.
	UINT				RECVDETAILLOG;		// �α� ���� �ڵ�
	UINT				RECVCALLINFO;		// message
	UINT				MessageShow;		// �۷ι� �޼��� �ڵ� 
	
	//CRITICAL_SECTION	CriticalLogWite;
	
	//HANDLE				hAcroIvrReadLog;	// �� �α׸� �͹̳��� �о����� ���θ� �� �� �ִ� �̺�Ʈ �ڵ鷯

	HANDLE				hMutex;				// 2�� ���� ���� 
	// IVR ���μ����� ����
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
