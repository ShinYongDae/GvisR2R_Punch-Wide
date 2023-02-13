#pragma once


// CDlgMenu06 대화 상자입니다.

class CDlgMenu06 : public CDialog
{
	DECLARE_DYNAMIC(CDlgMenu06)

public:
	CDlgMenu06(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMenu06();

// 대화 상자 데이터입니다.
// Dialog Data
//{{AFX_DATA(CDlgMenu06)
	enum { IDD = IDD_DLG_MENU_06 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

//#ifdef AFX_DESIGN_TIME
//	enum { IDD = IDD_DLG_UTIL_08 };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
};
