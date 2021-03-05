#if !defined(AFX_IVRPASOR_H__C7E163A3_BFAA_4AAA_BD9D_FA58030526E0__INCLUDED_)
#define AFX_IVRPASOR_H__C7E163A3_BFAA_4AAA_BD9D_FA58030526E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IVRPasor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIVRPasor window

typedef struct tagCOMMANDSTRUCT{
	int	linecount;
	int linediv[100];
}COMMANDSTRUCT;


typedef struct tagCOMMANDQUE{
	int			commandcount;
	CString		str[30];	
}COMMANDQUE;

//int channel, int state, CString sE164, CString dE164, int sCenarioStep)

class CCallStateBuf
{
public:
	CCallStateBuf();
	~CCallStateBuf();
	// function
	void			init();
	void			free();
	void			build(int iCallNum,CString src,CString dst,int step,int st);
	int				ParsePacket(char *packet);
	// operator
	CCallStateBuf	&operator=(const CCallStateBuf &c);
	int				operator==(const CCallStateBuf &c);
	int				operator<(const CCallStateBuf &c);
	int				operator>(const CCallStateBuf &c);
	// variable
	int				iCallNum;
	CString			sE164;
	CString			dE164;
	int				iStep;
	int				state;
};


class CIVRParsor : public CWnd
{
// Construction
public:
	CIVRParsor();
	
	char m_pSendBuff[1580];
	char m_pRecvBuff[1580];
	int	 m_nSendLength;
	int	 m_nSockStatus;		// 0 - default, 1 - command send/recv , 2- data recv
	int  m_nComSendCount;
	void ParsePacket(char *packet);
	COMMANDSTRUCT	LineAnaly( char *buff );
	COMMANDQUE		pCOMQUE;
	void SetCallStatus(char *buff );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIVRParsor)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIVRParsor();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIVRParsor)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IVRPASOR_H__C7E163A3_BFAA_4AAA_BD9D_FA58030526E0__INCLUDED_)
