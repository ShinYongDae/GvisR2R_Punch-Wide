// DlgMenu06.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GvisR2R_Punch.h"
#include "DlgMenu06.h"
//#include "afxdialogex.h"


#include "../GvisR2R_PunchDoc.h"
#include "../GvisR2R_PunchView.h"

extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

// CDlgMenu06 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMenu06, CDialog)

CDlgMenu06::CDlgMenu06(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLG_MENU_06, pParent)
{

}

CDlgMenu06::~CDlgMenu06()
{
}

void CDlgMenu06::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMenu06, CDialog)
END_MESSAGE_MAP()


// CDlgMenu06 메시지 처리기입니다.

