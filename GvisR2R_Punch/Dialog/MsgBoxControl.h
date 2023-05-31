#pragma once

#include "DlgMsgBox.h"
#include "../MailSlot/SyncMailslotWriter.h"
#include "MySharedMem.h"

#define WM_CONTROL_MESSAGE_BOX (WM_USER+130)

typedef struct MyMessage{
	CString strTitleMsg;
	CString strMsg;
	COLORREF color;
	DWORD dwDispTime;
	BOOL bOverWrite;
	CString strErrorDispatchMsg;
	ULONGLONG nGlobalID;
}structMyMessage;

typedef enum tagCmdListMsg
{
	MSG_CMD_MSG_CALL,
	MSG_CMD_MSG_DESTROY
}MSG_CALL_CMD;


typedef struct tagMsgCmd
{
	MSG_CALL_CMD nCmd;
	structMyMessage* pMsg;
}MSG_CMD;

class CMessageBoxControl : public CWinThread, CSyncMailslotWriter
{
	DECLARE_DYNCREATE(CMessageBoxControl)

public:
	CMessageBoxControl();
	~CMessageBoxControl();

public:

	BOOL IsMsgWindowVisible();

	BOOL m_bModal;
	CMySharedMem m_MsgBoxState;
	CString m_strTitle;// = strTitleMsg;
	CString m_strMsg;// = strMsg;
	ULONGLONG m_DispTime;// = dwDispTime;
	CString m_strErrorMsg;// = strErrorGuide;
	COLORREF m_TextColor;// = color;

	void DoModal(CString strTitle, CString strMsg, ULONGLONG dwInMilliSeconds, COLORREF color, CString strError);

	void OnProcMsg(WPARAM wParam, LPARAM lParam);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	ULONGLONG m_nGlobalMsgID;
	CCriticalSection m_csGarbage;
	std::vector< structMyMessage*>m_GarbageListMsg;


	BOOL CreateDialogMsg();
	void DestroyControl();

	CDlgMsgBox m_MessageBox;

	BOOL m_bMsgBoxAlive;

	//std::shared_future<void>m_TaskEnd;
	BOOL m_bThreadEndCall;
	CCriticalSection m_cs;

	//std::queue<MSG_CMD>m_CmdList;
	
	void CallMessage(structMyMessage* pMessage);
	void ClearDispMessage();
	//void SetWindowHandle(HWND hwnd);
	HWND m_hWnd;

	//void DoLoop();
protected:
	DECLARE_MESSAGE_MAP()
};