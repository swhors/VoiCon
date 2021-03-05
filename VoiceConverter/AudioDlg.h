// AudioDlg.h : header file
//

#if !defined(AFX_AUDIODLG_H__93EB780D_A544_4134_AC7B_A96EE5E561D5__INCLUDED_)
#define AFX_AUDIODLG_H__93EB780D_A544_4134_AC7B_A96EE5E561D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AcmG711a.h"
#include "AcmG711u.h"
#include "AcmG7231.h"

//#include "Audiodef.h"
/////////////////////////////////////////////////////////////////////////////
// CAudioDlg dialog

class CAudioDlg : public CDialog
{
// Construction
public:
	void FileDir();
	CAudioDlg(CWnd* pParent = NULL);	// standard constructor
	~CAudioDlg();
	
	void			filelistInit();
	void			FindFile(LPSTR lpszCurPath);
	void			MenuSel(DWORD dwParam);
	static void CALLBACK WaveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 );
	void			get_acm_info();
	
	int				iDsLength;
	char			cCurDrive;
	TCHAR			lpszOldPath[MAX_PATH];
	DRIVERINFO		driverInfo;
	HACMSTREAM		m_hStream;
	HACMDRIVER		m_hDriver;
	BYTE			*szSrcdataBuffer;
	BYTE			*szDsdataBuffer;
	HACMDRIVERID	DeviceId;
	HACMDRIVERID	pcmID;
	int				iSrcLength;
	int				iSelectedItem;
	char			szSrcFile[MAX_PATH];
	char			szDsFile[MAX_PATH];
	int				outputSubType;	// sub format : wave , raw 
									// wave - 1 raw - 0
	int				outputType;		// main format : g7231, g711...
									// g7231 - 0 g711u - 1 g711a - 2
	int				inputType;		// input format, this is now only support one type format, wave...
									// wave - 0
	int				iTemCount;

	// wave play info file
	//WAVE_PLAY_INFO playInfo;


// Dialog Data
	//{{AFX_DATA(CAudioDlg)
	enum { IDD = IDD_AUDIO_DIALOG };
	CComboBox		m_cb_drive;
	CListCtrl		m_idc_file;
	CListBox		m_list_path;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	void			StartWaveToGXX();
	void			OnBtnOpenSrc();

// Implementation
protected:
	HICON			m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CAudioDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT UpdateItemText(WPARAM wParam,LPARAM lParam);
	virtual void OnOK();
	afx_msg void OnBtnAbout();
	afx_msg void OnDblclkListPath();
	afx_msg void OnRclickFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCbDrive();
	afx_msg void OnMenuSel();
	afx_msg void OnBtnAll();
	afx_msg void OnG729a();
	afx_msg void OnRefresh();
	afx_msg void OnBtnG729All();
	afx_msg void OnColumnclickFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIODLG_H__93EB780D_A544_4134_AC7B_A96EE5E561D5__INCLUDED_)
