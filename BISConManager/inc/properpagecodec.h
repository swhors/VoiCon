#if !defined(AFX_PROPERPAGECODEC_H__A7636A71_42C5_46B2_B454_F32434709BFF__INCLUDED_)
#define AFX_PROPERPAGECODEC_H__A7636A71_42C5_46B2_B454_F32434709BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// properpagecodec.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProperPageCodec dialog

class CProperPageCodec : public CPropertyPage
{
	DECLARE_DYNCREATE(CProperPageCodec)

// Construction
public:
	void EnableUinputTimeOutCtl(BOOL bVal);
	void WriteAudioInfo(IN int Count,IN int AudioCodec);
	void ChkG7231Audio(CComboBox & cb);
	void EnableG7231Item(BOOL bVal=FALSE);
	void SetCBItem(CComboBox &cb,int type=0);
	void EnableCommonItem(BOOL bVal=FALSE);
	void SetCodecInfo(IN int count=0,IN int iCodecType=0);
	CProperPageCodec();
	~CProperPageCodec();

// Dialog Data
	//{{AFX_DATA(CProperPageCodec)
	enum { IDD = IDD_PROPPAGE_CODEC };
	CEdit	m_ctUserInputReady;
	CEdit	m_ctFirstUserInputReady;
	CButton	m_chk3;
	CButton	m_chk2;
	CButton	m_chk1;
	CComboBox	m_ctCBAudio3;
	CComboBox	m_ctCBAudio2;
	CComboBox	m_ctCBAudio1;
	CButton	m_ctChkDTMFEnable;
	CButton	m_ctChkSilenceSupression;
	CEdit	m_ctAudioFrame;
	CComboBox	m_ctCbDTMFType;
	int		m_RtpPort;
	BOOL	m_audioRecord;
	BOOL	m_rearrangedRTP;
	int		m_mentInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProperPageCodec)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProperPageCodec)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkdtmfenable();
	afx_msg void OnEditchangeCbaudio1();
	afx_msg void OnEditchangeCbaudio2();
	afx_msg void OnEditchangeCbaudio3();
	afx_msg void OnSelchangeCbaudio1();
	afx_msg void OnSelchangeCbaudio2();
	afx_msg void OnSelchangeCbaudio3();
	afx_msg void OnChk1();
	afx_msg void OnChk2();
	afx_msg void OnChk3();
	afx_msg void OnSelchangeCbdtmftype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERPAGECODEC_H__A7636A71_42C5_46B2_B454_F32434709BFF__INCLUDED_)
