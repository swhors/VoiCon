#if !defined(AFX_TERMINAL_H__DFCD56F6_7163_4BAC_9F9A_4D65B714BFB3__INCLUDED_)
#define AFX_TERMINAL_H__DFCD56F6_7163_4BAC_9F9A_4D65B714BFB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Terminal.h : header file
//
#include "TerminalAck.h"
#include "TerminalDef.h"

/////////////////////////////////////////////////////////////////////////////
// CTerminal command target

class CTerminal : public CAsyncSocket
{
// Attributes
public:
	CTerminalAck ct[TERMIANL_NUM];
	int iConnectCount;
// Operations
public:
	CTerminal();
	virtual ~CTerminal();

// Overrides
public:
	void SendCallLog(char * sBuffer, int iCallNum);
	void SendDetailLog(char * sBuffer, int iCallNum);
	int GetFreeTerminal();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminal)
	public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTerminal)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINAL_H__DFCD56F6_7163_4BAC_9F9A_4D65B714BFB3__INCLUDED_)
