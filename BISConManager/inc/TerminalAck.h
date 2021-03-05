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
	BOOL	bUserID; // ������� �α��� ���̵� ��ġ������ ���� �÷���
	BOOL	bUserPw; // ������� ��ȣ�� ��ġ������ ���� �÷���
	BOOL bDblEnterDlog;	// �󼼷αװ� TRUE�� ���, ���� Ű�� �Է� �Ǿ��� ���, 
						//�󼼷α׸� �Ͻ� FALSE�� ���� ���� ���� ����.
	BOOL bDblEnterClog;	// �� �αװ� TRUE�� ���, ���� Ű�� �Է� �Ǿ��� ���, 
						//�ݷα׸� �Ͻ� FALSE�� ���� ���� ���� ����.
	int iCommanBuffer;  // 
	char command_history[10][1024]; // ������� Ŀ�ǵ� �Է��� ����ϴ� ����
	BOOL bDetailLog; // IVR�� �� �α׸� �� �������� ����.
	BOOL bCallLog; // �ݿ� ���� ���� �α׸� �� ������ ����.
	int iLeft;  // �������� Ŀ���� �̵��� ���, �̵� �ݺ� Ƚ��.
	int iRight; //���������� Ŀ���� �̵��� ���, �̵� �ݺ� ȸ��.
	char sReceiveBuf[1024]; // ����ڰ� �Է��� ���ڸ� ���� ���ڰ� �ԷµǱ� ���� �����ϴ� ����.
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
	//������� ��ȣ�� ����.
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
