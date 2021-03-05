#if !defined(AFX_VOICERECPAGE1_H__CFE564C5_AF60_4867_AA7B_57141A21AA19__INCLUDED_)
#define AFX_VOICERECPAGE1_H__CFE564C5_AF60_4867_AA7B_57141A21AA19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// voicerecpage1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVoiceRecPage1 dialog

class CVoiceRecPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CVoiceRecPage1)

// Construction
public:
	CVoiceRecPage1();
	~CVoiceRecPage1();

// Dialog Data
	//{{AFX_DATA(CVoiceRecPage1)
	enum { IDD = IDD_VOICEREC_PAGE1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVoiceRecPage1)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CVoiceRecPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICERECPAGE1_H__CFE564C5_AF60_4867_AA7B_57141A21AA19__INCLUDED_)
