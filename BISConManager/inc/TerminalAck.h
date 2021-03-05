#if !defined(AFX_TERMINALACK_H__69AE98C1_3941_43B6_BCCC_B5F8030CFF06__INCLUDED_)
#define AFX_TERMINALACK_H__69AE98C1_3941_43B6_BCCC_B5F8030CFF06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerminalAck.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CTerminalAck command target

class CTerminalAck : public CAsyncSocket
{
// Attributes
public:
	BOOL	bUserID; // 사용자의 로그인 아이디가 일치한지의 상태 플래그
	BOOL	bUserPw; // 사용자의 암호가 일치한지의 상태 플래그
	BOOL bDblEnterDlog;	// 상세로그가 TRUE일 경우, 엔터 키가 입력 되었을 경우, 
						//상세로그를 일시 FALSE로 놓기 위한 상태 변수.
	BOOL bDblEnterClog;	// 콜 로그가 TRUE일 경우, 엔터 키가 입력 되었을 경우, 
						//콜로그를 일시 FALSE로 놓기 위한 상태 변수.
	int iCommanBuffer;  // 
	char command_history[10][1024]; // 사용자의 커맨드 입력을 기억하는 버퍼
	BOOL bDetailLog; // IVR의 상세 로그를 볼 것인지를 결정.
	BOOL bCallLog; // 콜에 관한 접속 로그를 볼 것인지 결정.
	int iLeft;  // 왼쪽으로 커서를 이동할 경우, 이동 반복 횟수.
	int iRight; //오른쪽으로 커서를 이동할 경우, 이동 반복 회수.
	char sReceiveBuf[1024]; // 사용자가 입력한 문자를 엔드 문자가 입력되기 까지 저장하는 버퍼.
	int iTerminalState; // 1=Using 0=NotUsing
	int iTerminalNum;	
	char szUserPW[32];
	char szUserID[32];
// Operations
public:
	CTerminalAck();
	virtual ~CTerminalAck();
	int initValue();
	void ViewScenarioFile(CString csArg);
	void ViewScenarioFileList(CString & csResponse);
	void CommandServiceParse(char * sbuffer);
	void CommandLogParse(char * sArg, CString &csResponse);	
	void WriteConf(int iItem, char *pArg, int iReserved, CString & csResponse);
	void SetConfigure(CString csItem,CString &csResponse);
	void ViewConfigure(CString &csResponse);
	void CommandIvrParse(char * sArg, CString &csResponse,int iVal=0);
	void CommandHelpParse(char *sArg,CString &csResponse);
	void CommandViewParse(char *sArg,CString &csResponse);
	int	ParseCommand(char * sCommand);
	//사용자의 암호를 변경.
	void ModifyPassword(CString csUserID);
	void AdminUser(int iSubCommand, CString csCommand, CString csResponse);
// Overrides
public:
	int iFindUserID(int &iCount);
	void WriteIDnPasswd();
	BOOL GetUserInfo();
	void ModifyPassword();
	BOOL bInputPasswd;
	int iRetrialCount;
	BOOL bVerifyPasswd();
	BOOL bVerifyUser();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminalAck)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTerminalAck)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALACK_H__69AE98C1_3941_43B6_BCCC_B5F8030CFF06__INCLUDED_)
