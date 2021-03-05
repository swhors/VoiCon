#pragma once


// CHubPage1 대화 상자입니다.

class CHubPage1 : public CPropertyPage
{
	DECLARE_DYNAMIC(CHubPage1)

public:
	CHubPage1();
	virtual ~CHubPage1();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_HUB_PAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	virtual void OnOK();
public:
	virtual void OnCancel();
};
