// Terminal.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Terminal.h"
//#include "TerminalDef.h"
//#include "IvrRemoteCtrlDlg.h"
#include "BISConManager.h"
#include "BISConManagerDoc.h"
#include "StateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CBISConManagerApp theApp;

extern void LogWriteTerminalOperate(char * slog0, char * slog1, int i0, int i1);

/////////////////////////////////////////////////////////////////////////////
// CTerminal

CTerminal::CTerminal()
{
	iConnectCount = TERMIANL_NUM;
	for( int i = 0; i< iConnectCount;i ++)
	{
		ct[i].iTerminalNum = i;
	}
	Create(23);
	Listen();
}

CTerminal::~CTerminal()
{

}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CTerminal, CAsyncSocket)
	//{{AFX_MSG_MAP(CTerminal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CTerminal member functions
void CTerminal::OnAccept(int nErrorCode)
{	
	CString csSendbuffer;
	//CIVRRemoteCtrlDlg * pDlg = (CIVRRemoteCtrlDlg * )AfxGetApp()->m_pMainWnd;
	int iTerminal = GetFreeTerminal();
	CAsyncSocket rcv;
	if( iTerminal == -1)
	{
		Accept(rcv);
		rcv.Send(ERROR_OVER_ALLOWED, strlen(ERROR_OVER_ALLOWED));
		rcv.Close();
		return ;
	}
	if( Accept(ct[iTerminal]) != 0)
	{
		ct[iTerminal].Send(WELCOM_STRING, strlen( WELCOM_STRING));
#ifdef _AUTH_TERMINAL
		csSendbuffer.Format("Max Allowed Call:%d\r\n", pDlg->iMaxAllowedCall);
		ct[iTerminal].Send(csSendbuffer, strlen( csSendbuffer));
		ct[iTerminal].Send(LOGIN, strlen( LOGIN));
#else // _AUTH_TERMINAL
		csSendbuffer.Format("Max Allowed Call:%d\r\n>", theApp.m_pIVR_System.Ivr_MaxCall);
		ct[iTerminal].Send(csSendbuffer, strlen( csSendbuffer));
#endif // _AUTH_TERMINAL
	}
	CAsyncSocket::OnAccept(nErrorCode);
}

void CTerminal::OnClose(int nErrorCode) 
{
	CAsyncSocket::OnClose(nErrorCode);
}

int CTerminal::GetFreeTerminal()
{
	for( int i = 0; i < iConnectCount ; i++)
	{
		if( ct[i].iTerminalState == 0)
		{
			ct[i].iTerminalState = 1;
			return i;
		}
	}
	return -1;
}

void CTerminal::SendDetailLog(char *sBuffer, int iCallNum)
{
	CString SendBuf = sBuffer;
	SendBuf.Replace("\n", "\r\n");
	for( int i = 0 ; i < iConnectCount ; i++)
	{
		if( ct[i].iTerminalState == 1)
		{
			if( ct[i].bDetailLog == TRUE)
			{
				ct[i].Send(SendBuf, SendBuf.GetLength(), 0);
				//ct[i].Send(">", 1, 0);
			}
		}
	}
}

void CTerminal::SendCallLog(char *sBuffer, int iCallNum)
{
	CString SendBuf = sBuffer;
	if( SendBuf.Compare("#") )
		SendBuf +="\r\n";
	for( int i = 0 ; i < iConnectCount ; i++)
	{
		if( ct[i].iTerminalState == 1)
		{
			if( iCallNum != -1)
			{
				if( ct[i].bCallLog == TRUE)
				{
					ct[i].Send(SendBuf, SendBuf.GetLength(), 0);
					//ct[i].Send(">", 1, 0);
				}
			}
			else
			{
				ct[i].Send(SendBuf, SendBuf.GetLength(), 0);
				//ct[i].Send(">", 1, 0);
			}
		}
	}
}