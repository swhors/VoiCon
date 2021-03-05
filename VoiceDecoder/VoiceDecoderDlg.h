// VoiceDecoderDlg.h : header file
//

#if !defined(AFX_VOICEDECODERDLG_H__72CBF336_EB25_41EF_85C1_4779365CC30B__INCLUDED_)
#define AFX_VOICEDECODERDLG_H__72CBF336_EB25_41EF_85C1_4779365CC30B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVoiceDecoderDlg dialog

class CVoiceDecoderDlg : public CDialog
{
// Construction
public:
	void WaveMix(IN short *in1,IN short *in2,OUT short *out,IN int nSize1,IN int nSize2,IN int nPreMargineSec,IN int nSamplesPerSec);
	BOOL VoiceFileOpen(CString csFileName, CFile &ctrFile,PBYTE & ptrOutBuf,int &dLen);
	BOOL WaveWrite(IN CString fileName,IN PBYTE data,IN int len,IN char *strOtherName=NULL);
	int acmConvert(IN PBYTE str_iData,IN int i_iLen,IN OUT PBYTE str_oData,IN OUT int  i_oLen);
	void acmClose();
	BOOL acmOpen();
	void DecodeVoice();
	CVoiceDecoderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVoiceDecoderDlg)
	enum { IDD = IDD_VOICEDECODER_DIALOG };
	CButton	m_btnResetTitle;
	CButton	m_btnDecode;
	CButton	m_btnMix;
	CButton	m_btnOpen2;
	CEdit	m_ctFileName2;
	CEdit	m_ctFileName1;
	CString	m_strFileName;
	CString	m_strFileName2;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceDecoderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVoiceDecoderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnOpen();
	afx_msg void OnBtnDecode();
	afx_msg void OnBtnOpen2();
	afx_msg void OnBtnMix();
	afx_msg void OnBtnAbout();
	afx_msg void OnChangeEdFilename2();
	afx_msg void OnBtnResettitle();
	afx_msg void OnChangeEdFilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICEDECODERDLG_H__72CBF336_EB25_41EF_85C1_4779365CC30B__INCLUDED_)
