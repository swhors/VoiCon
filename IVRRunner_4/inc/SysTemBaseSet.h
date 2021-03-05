// Global Variables:
extern	HINSTANCE		hInst;	// current instance
extern	HWND			m_hWnd;

// 프로그램의 힙 영역 핸들
//extern HANDLE hHeap;

extern	char			*sCodec[5];

// Ivr default information
extern	BOOL			bEnableCallCoupl;
extern	int				iMaxAllowedCall;
extern	int				iInstalledLang;
extern	TCHAR			sLanguage[8][4];
//extern	BOOL			bAutoRun; // IVR을 자동으로 시작할 것인지를 결정.

//extern	int				iIvrDefaultSilenceSupression;

// 음성 파일과 관련된 변수
extern	PVOICE_PAGE		pVoicePage; //음성 파일의 페이지
						//음성 파일의 하위폴더별로 정보를 가짐.
extern	int				iVocePageCount; // 음성 파일 페이지의 수

// Scenario Information
extern	int				sf_count; // scenario file count;
//extern	TCHAR			scenario_file[10][48];// 로딩할 시나리오 파일의 개 수. <최대 4개>
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

// IVR 프로그램의 메인 시작 위치;
extern char				szIvrMainPath[256];
