#if !defined(AFX_INPUTPAGE1_H__FA6CFF0E_5ED3_4224_A065_1190C8089E0E__INCLUDED_)
#define AFX_INPUTPAGE1_H__FA6CFF0E_5ED3_4224_A065_1190C8089E0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputPage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputPage1 dialog

class CInputPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CInputPage1)
// Construction
public:
	void init_cb(int iSelectedItem=-1);
	CInputPage1();
	~CInputPage1();
// Dialog Data
	//{{AFX_DATA(CInputPage1)
	enum { IDD = IDD_INPUT_PAGE_1 };
	CComboBox	m_cb_arg;
	CEdit	m_edBufLength;
//	CButton	m_rd5;
//	CButton	m_rd4;
//	CButton	m_rd3;
//	CButton	m_rd2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInputPage1)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInputPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTPAGE1_H__FA6CFF0E_5ED3_4224_A065_1190C8089E0E__INCLUDED_)
