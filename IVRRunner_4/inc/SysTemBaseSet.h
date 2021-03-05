// Global Variables:
extern	HINSTANCE		hInst;	// current instance
extern	HWND			m_hWnd;

// ���α׷��� �� ���� �ڵ�
//extern HANDLE hHeap;

extern	char			*sCodec[5];

// Ivr default information
extern	BOOL			bEnableCallCoupl;
extern	int				iMaxAllowedCall;
extern	int				iInstalledLang;
extern	TCHAR			sLanguage[8][4];
//extern	BOOL			bAutoRun; // IVR�� �ڵ����� ������ �������� ����.

//extern	int				iIvrDefaultSilenceSupression;

// ���� ���ϰ� ���õ� ����
extern	PVOICE_PAGE		pVoicePage; //���� ������ ������
						//���� ������ ������������ ������ ����.
extern	int				iVocePageCount; // ���� ���� �������� ��

// Scenario Information
extern	int				sf_count; // scenario file count;
//extern	TCHAR			scenario_file[10][48];// �ε��� �ó����� ������ �� ��. <�ִ� 4��>
extern	int				iInputWaitTime;
extern	int				iInputInitWaitTime;
extern	SCM				*scm;
// H323 Information
#define H323BUFFERLEN	32
extern	TCHAR			szH323ID[H323BUFFERLEN];
extern	TCHAR			szE164ID[32];
extern	TCHAR			szGlobalGkIp[24];
extern	int				iGkPort;
extern	int				iMaxDelayJitter; // not used
extern	int				iTTL; // not used 
extern	HAPP			hApp;
extern	int				m_iFastStart;
extern	int				m_iH245Tunneling;

// Plugin Information.
extern	HANDLE_BUFFER	handleBuffer[10];
extern	int				iDllCount;

// Call Information
extern	CONNECTION		pCon[400];
//extern	RTPCHECKBUFF	pRTPCHEKCBUFF[200];
// License Information
extern	BOOL			bIsUSBEnable;
extern	IVRFILELOCK		keyData;

// special codec support
extern	int				m_g729Support;

// rtp Port
extern	int				m_InitOfRtpPort;

// IVR ���α׷��� ���� ���� ��ġ;
extern char				szIvrMainPath[256];
