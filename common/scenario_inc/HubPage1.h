#pragma once


// CHubPage1 ��ȭ �����Դϴ�.

class CHubPage1 : public CPropertyPage
{
	DECLARE_DYNAMIC(CHubPage1)

public:
	CHubPage1();
	virtual ~CHubPage1();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_HUB_PAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	virtual void OnOK();
public:
	virtual void OnCancel();
};
