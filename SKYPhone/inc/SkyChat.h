#pragma once
#include "afxwin.h"

#include "SkinButton.h"
#include "EditEx.h"
#include "SkinLabel.h"

// CSkyChat 대화 상자입니다.

class CSkyChat : public CDialog
{
	DECLARE_DYNAMIC(CSkyChat)

public:
	char    target_num[64];
	char    u_input_str[512];
	CSkinFont	m_CalledFont;	
	CSkyChat(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSkyChat();
// Dialog Data
	//{{AFX_DATA(CSkyChat)
	CString	m_strMsg;
	CSkinLabel	m_lblCalledNo;
	//}}AFX_DATA

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_CHAT };
    void InitSkinHomeDir();
    BOOL InitSkinBkGnd( char *BkGndFile );
	void InitSkinFont();
    void InitSkinBtn( CSkinButton &Btn, int x, int y, int w, int h, CString &FileName );
	void	InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, CSkinFont *SkinFont );
	void	InitSkinLabel( CSkinLabel &Label, int x, int y, int w, int h, t_Font *SkinFont );
protected:
	char	m_SkinDir[100];
	CBitmap m_bmpBkGnd;	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	// Generated message map functions
	//{{AFX_MSG(CSmsMainDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnMsgChatRcv(WPARAM wParam,LPARAM lParam);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSkinButton m_close;
	CSkinButton m_min;
	CSkinButton m_symbol;
	CEditEx m_view_wnd;
	CEditEx m_talk;
	CListBoxEx m_lstWnd;	
	//CEditEx m_num;
	void init_skin(void);
	CSkinButton m_send;
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnSymbol1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnCancel();
public:
	virtual BOOL DestroyWindow();
};
extern CSkyChat *pChatDlg;