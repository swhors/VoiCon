// UbiDialerDlg.h : header file
//

#if !defined(AFX_UBIDIALGERDLG_H__708A6430_A510_42A5_BAF6_66373B45D349__INCLUDED_)
#define AFX_UBIDIALERDLG_H__708A6430_A510_42A5_BAF6_66373B45D349__INCLUDED_

#include "H323_INC\CallConf.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SkinButton.h"
#include "SkinLabel.h"
#include "SkinSlide.h"
#include "mmsystem.h"
#include "afxwin.h"
#ifdef SIP_UA
#include "sipmain.h"
#endif
#define WM_CHAT_MSGSD        WM_USER+3000
#define WM_CHAT_MSGRV        WM_USER+3001
#define WM_CHAT_DESTROY		 WM_USER+3002


#define DEFAULT_AUDIO_DEVICE	"USB Audio Device        "
#define		CHATTING_DIR		".\\bmp\\03_sub\\03_09_chatting"
class		CSCUsbPhone;

#include "H323CallInfo.h"
#include "CallConf.h"
#include "INC\WaveCtrl.h"	// Added by ClassView


/////////////////////////////////////////////////////////////////////////////
// CUbiDialerDlg dialog
class CUbiDialerDlg : public CDialog
{
// Construction
public:
	CUbiDialerDlg(CWnd* pParent = NULL);	// standard constructor
	~CUbiDialerDlg();

public:
	char           chat_msg[512];
	HMODULE        hUSBModule;
	BOOL           m_bConnect;
#ifdef SIP_UA
	CSIPMain       *m_pPhone;
#endif
	CH323CallInfo	cH323CallInfo;
	CString			m_sExeDir;
	CString			m_strGK;
	CString			m_strID;
	CString			m_strPN;
	CString			m_strAmount;
	CString			m_strTime;
	BOOL			bMicMute;
	BOOL			bSpeakerMute;
	
	//t_Dial		m_Dial[10];
	char			m_szPhone[30];
	char			m_szUsedNumber[10][64];
	int				iLastUsedNumber;
	int             iCurUsedNumber;

	HMIXER			hmx;		
	HMIXER			hMixer, hMixer1; // Mixer handle used in mixer API calls. 
	MIXERCONTROL	mxc, mxc1; // Holds the mixer control data. 
	DWORD			dwValue_head; 
    int    CheckUpdate();
    void    CtlUI(BOOL opt);
	void	GetExeDir(LPSTR lpCurDir, DWORD dwSize);
	void	SpekerVolControl(int value);
	void	MicVolControl(int value) ;
	int		GetMixerControl(HMIXER hMixer, DWORD dwDstType, DWORD dwSrcType,DWORD dwCtrlType, MIXERCONTROL* pmxc);
	int		GetDestLineIndex(HMIXER hMixer, MIXERLINE &line, DWORD lineType);
	int		GetSourceLineIndex(HMIXER hMixer, MIXERLINE &line, DWORD lineType);
	int		GetSourceLineInfo(HMIXER hMixer, MIXERLINE &line, int index);
	int		SetMixerValue(HMIXER hMixer, MIXERCONTROL* pmxc, DWORD* pValue);
	// USB 디바이스의 IOCTL 제어 함수
	int UsbDeviceIOCTL(DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);
	void GetWaveOutVolume();

	BOOL findAudioDevice(LPWAVEOUTCAPS t_waveCaps, int *iDev);
	void SetWaveOutVolume(int iPos);
	void GetWaveInVolume();
	BOOL findAudioInDevice(LPMIXERCAPS t_mixerCaps, int *iDev);
	void SetWaveInVolume(int iPos);
	void MicMute();
	void SpeakerMute();
// buzz
	void hid_buzz_play_on();
	void hid_buzz_play_off();
	void hid_buzz_on();
	void hid_buzz_off();
	void findHidDevice();
	char devicePath[1024];
	HANDLE hidWriteDevice;
	DWORD writeLen;
	unsigned char hidTelegram[17];
// Dialog Data
	//{{AFX_DATA(CUbiDialerDlg)
	enum { IDD = IDD_UBIDIALER_DIALOG };
	CSkinButton	m_btnSms;
	CSkinLabel	m_lblDur;
	CSkinLabel	m_lblAmount;
	CSkinButton	m_btnHistory;
	CSkinButton	m_btnBook;
	CSkinButton	m_btnChat;
	CSkinSlide	m_sldSpk;
	CSkinSlide	m_sldMic;
	CSkinLabel	m_lblCallerNo;
	CSkinLabel	m_lblTime;
	CSkinLabel	m_lblMsg;
	CSkinLabel	m_lblMid;
	CSkinButton	m_imgMic;
	CSkinButton	m_imgSpk;
	CSkinButton	m_btnMini;
	CSkinButton	m_btnHelp;
	CSkinButton	m_btnClose;	
	CSkinButton	m_btnStar;
	CSkinButton	m_btnSharp;
	CSkinButton	m_btn9;
	CSkinButton	m_btn8;
	CSkinButton	m_btn7;
	CSkinButton	m_btn6;
	CSkinButton	m_btn5;
	CSkinButton	m_btn4;
	CSkinButton	m_btn3;
	CSkinButton	m_btn2;
	CSkinButton	m_btn1;
	CSkinButton	m_btn0;
	CSkinButton	m_btnSend;
	CSkinButton	m_btnClear;
	CSkinButton	m_btnEnd;
	CSkinButton	m_btnConf;
	CSkinButton m_btnTransfer;
	CSkinButton m_recharge;

	CStatic m_antena;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUbiDialerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CBitmap m_bmpBkGnd;
	CSkinFont	m_TopFont;	
	CSkinFont	m_TimeFont;	
	CSkinFont	m_MidFont;
	CSkinFont	m_DownFont;
	t_Font		m_TextFont;
	
	// 콜 정보
	t_Talk  TalkBuff;
	time_t	call_start_time;

	int		check_time_lock();
	int		GetVersionInfo(CString &csBuf);

	void    ViewAntena();
	void	InitSkin();
	void	InitSkinFont();
	void	InitSkinBkGnd( char *BkGndFile );
	void	InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, CString &FileName );
	void	InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, CSkinFont *SkinFont );
	void	InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, t_Font *SkinFont );
	void	InitSkinSlide( CSkinSlide &Slide, int x, int y, int w, int h, 
			CString &HeadFile, int Head_h, CString &BodyFile, int Body_h, CString &TailFile, int Tail_h );

	void	GetNowTime();
	void	SaveRecentTalk( t_Talk *pTalk );

	void    OnConnected(int direction);
	void    OnDisConnected(int code = 0);

	void	Start_Talk(int direction);
    void	End_Talk(int opt);

	// Generated message map functions
	//{{AFX_MSG(CUbiDialerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnBtn0();
	afx_msg void OnBtn1();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBtnClose();	
	afx_msg void OnMidLabel();
	afx_msg void OnBtn2();
	afx_msg void OnBtn3();
	afx_msg void OnBtn4();
	afx_msg void OnBtn5();
	afx_msg void OnBtn6();
	afx_msg void OnBtn7();
	afx_msg void OnBtn8();
	afx_msg void OnBtn9();
	afx_msg void OnBtnSt();
	afx_msg void OnBtnSh();
	afx_msg void OnBtnEnd();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnMini();
	afx_msg void OnMicImage();
	afx_msg void OnSpkImage();
	afx_msg void OnBtnBook();
	afx_msg void OnBtnTelHst();
	afx_msg void OnBtnSend();
	afx_msg LONG OnSlideChangeSpkVal(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnSlideChangeMicVal(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnRemainRecv(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnWmUserMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT IOSendMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWmChatSend(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChatDlgDown(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBtnSms();
    afx_msg LRESULT OnWmDownUSB(WPARAM wParam,LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnChat();
	afx_msg void OnBnClickedLabelAmount();
	afx_msg void OnBnClickedLabelDuration();
	afx_msg void OnBnClickedBtnConf();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CWaveCtrl waveCtl;
	CCallConf m_CallConf;
	int start_ua(void);
	void stop_ua(int opt);
	int bCalling;
	int bCalled;
	// USB 디바이이스의 DLL을 로드하여 핸들을 얻음
	int LoadUSBDeviceDll(void);
	void ReleaseUSBDeviceDll(void);
	void deleteLabel(void * Item);
	void append_used_num(char *u_num);
	int  get_used_num(char *u_num);
	afx_msg void OnBnClickedBtnRecharge();
};

extern	CUbiDialerDlg	*g_PhoneDlg;


//  endpoint h323id, endpoint e164(calling party number) , 
//  Client module이 Listen하고 있을 Q931 port, client module이 listen하고 있을 H245 port,
//  Client module이 받아 들일 rtp, rtcp port,
//  endpoint의 IP Address,
//  proxy ip address,
//  endpoint port count 다포트 이외의 장비는 무조건 1 이다.
//extern "C" __declspec(dllexport) int InitClientModule(char *h323id, char *e164, int local_q931, int local_h245, int local_rtp, int local_rtcp, char *local_ip_address, char *proxy_ip_address, int ep_count);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UBIDIALERDLG_H__708A6430_A510_42A5_BAF6_66373B45D349__INCLUDED_)
