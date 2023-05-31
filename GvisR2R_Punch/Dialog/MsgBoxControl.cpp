#include "stdafx.h"
#include "MsgBoxControl.h"

#include "../GvisR2R_Punch.h"
#include "../GvisR2R_PunchDoc.h"
#include "../GvisR2R_PunchView.h"

extern CGvisR2R_PunchApp* pApp;
extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

IMPLEMENT_DYNCREATE(CMessageBoxControl, CWinThread)

BEGIN_MESSAGE_MAP(CMessageBoxControl, CWinThread)
	ON_THREAD_MESSAGE(WM_CONTROL_MESSAGE_BOX, OnProcMsg)
END_MESSAGE_MAP()

void CMessageBoxControl::OnProcMsg(WPARAM wParam, LPARAM lParam)
{
	//if (g_bDestroy)
	//{
	//	return;
	//}

	if (wParam == 1)
	{
		structMyMessage* pMsg = (structMyMessage*)lParam;

		if (m_MessageBox.GetSafeHwnd())
		{
			if (pMsg)
			{
				//m_MessageBox.SetDispMsg(pMsg->strTitleMsg, pMsg->strMsg, pMsg->dwDispTime, pMsg->color, pMsg->strErrorDispatchMsg);
				m_MessageBox.SetDispMsg(pMsg->strMsg, pMsg->strTitleMsg, pMsg->dwDispTime, pMsg->color);
				m_MessageBox.ShowWindow(SW_SHOW);
				m_MessageBox.UpdateWindow();

				m_csGarbage.Lock();
				std::vector<structMyMessage*>::iterator iter;
				for (iter = m_GarbageListMsg.begin(); iter != m_GarbageListMsg.end(); iter++)
				{
					if ((*iter)->nGlobalID == pMsg->nGlobalID)
					{
						m_GarbageListMsg.erase(iter);
						break;
					}
				}
				delete pMsg;
				pMsg = NULL;
				m_csGarbage.Unlock();

			}
		}
		else
		{
			m_MessageBox.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if(m_MessageBox.GetSafeHwnd())
			m_MessageBox.ShowWindow(SW_HIDE);
	}

	return;
}

CMessageBoxControl::CMessageBoxControl()
{
//	CreateDialogMsg();
	m_nGlobalMsgID = 0;
	m_bModal = 0;


	if(m_MsgBoxState.Open(_T("GvisMsgBoxShow"))!=1)
		m_MsgBoxState.CreateMem(_T("GvisMsgBoxShow"));

}

BOOL CMessageBoxControl::InitInstance()
{
	if (!m_bModal)
	{
		//m_MessageBox.m_bUIThread = 1;
		if (m_MessageBox.GetSafeHwnd() == 0)
			m_MessageBox.Create();
	}
	else
	{
		//m_MessageBox.m_bUIThread = 1;
		//m_MessageBox.m_bDoModal = 1;
		//m_MessageBox.m_strTitle = m_strTitle;
		//m_MessageBox.m_strMsg = m_strMsg;
		//m_MessageBox.m_DispTime = m_DispTime;
		//m_MessageBox.m_strErrorMsg= m_strErrorMsg;
		//m_MessageBox.m_TextColor = m_TextColor;

		this->m_pMainWnd = &m_MessageBox;
		m_MessageBox.DoModal();
	}

	return TRUE;
}

BOOL CMessageBoxControl::CreateDialogMsg()
{
	return CreateThread(CREATE_SUSPENDED);
}
void CMessageBoxControl::DestroyControl()
{
	//if (pDoc->m_bCalibFocusAdjustActionFlag == TRUE)
	//	pDoc->m_bCalibFocusAdjustActiveFlag = FALSE;
	//if (pDoc->m_bCalibLightAdjustActionFlag == TRUE)
	//	pDoc->m_bCalibLightAdjustActiveFlag = FALSE;

	//// 120605 jsy	
	//if (pDoc->m_bROIImageCaptureActive == TRUE)
	//	pDoc->m_bROIImageCaptureCommand = FALSE;

	//if (pView->m_pDlgSmartSetup && pView->m_pDlgSmartSetup->m_bContinuousMode)
	//	pView->m_pDlgSmartSetup->m_bUseStop = TRUE;

	CGvisR2R_PunchApp* pApp = (CGvisR2R_PunchApp*)AfxGetApp();
	if(pApp)
		pApp->KillProcess(MSG_BOX_PATH);
	
	if(m_hThread)
		this->PostThreadMessage(WM_QUIT, 0, 0);
}

int CMessageBoxControl::ExitInstance()
{
	if (!m_bModal)
	{
		if (m_MessageBox.GetSafeHwnd())
		{
			m_MessageBox.DestroyWindow();
		}
	}
	return CWinThread::ExitInstance();
}

CMessageBoxControl::~CMessageBoxControl()
{
	m_csGarbage.Lock();
	for (int i = 0; i < m_GarbageListMsg.size(); i++)
	{
		delete m_GarbageListMsg[i];
	}
	m_GarbageListMsg.clear();
	m_csGarbage.Unlock();

	CSyncMailslotWriter::Disconnect();
}

BOOL CMessageBoxControl::IsMsgWindowVisible()
{
	if (CSyncMailslotWriter::IsOpen() == false)
	{
		return 0;
	}

	if (m_MsgBoxState.m_hMem != INVALID_HANDLE_VALUE)
	{
		if (m_MsgBoxState.m_Mem_start)
		{
			if (m_MsgBoxState.m_Mem_start[0] == 1)
				return 1;
		}
	}

	return 0;
}


void CMessageBoxControl::CallMessage(structMyMessage* pMessage)
{
	if (CSyncMailslotWriter::IsOpen() == false)
	{
		CFileFind finder;

		if (finder.FindFile(MSG_BOX_PATH) == 1)
		{
			SHELLEXECUTEINFO info;
			ZeroMemory(&info, sizeof(info));
			info.cbSize = sizeof(SHELLEXECUTEINFO);
			info.hwnd = NULL;
			info.lpVerb = _T("open");
			info.lpFile = MSG_BOX_PATH;
			info.lpParameters = NULL;
			info.lpDirectory = NULL;
			info.nShow = SW_HIDE;
			info.hInstApp = NULL;
			ShellExecuteEx(&info);
			Sleep(1000);
		}

		if (CSyncMailslotWriter::Connect(_T("GvisMsgBox")))
		{

		}
	}

	if (CSyncMailslotWriter::IsOpen() == 1)
	{
		TCHAR* szTitle =new TCHAR[256];
		memset(szTitle, 0, sizeof(TCHAR) * 256);
		_stprintf(szTitle, _T("%s"), pMessage->strTitleMsg);

		TCHAR* szMessage = new TCHAR[1024];
		memset(szMessage, 0, sizeof(TCHAR) * 1024);
		_stprintf(szMessage, _T("%s"), pMessage->strMsg);

		TCHAR* szErr = new TCHAR[1024];
		memset(szErr, 0, sizeof(TCHAR) * 1024);
		_stprintf(szErr, _T("%s"), pMessage->strErrorDispatchMsg);

		int nTotalSize = 256* sizeof(TCHAR) + 1024 * sizeof(TCHAR) + 1024 * sizeof(TCHAR) + sizeof(COLORREF) + sizeof(DWORD) + sizeof(BOOL) + 1;
		BYTE* pMemory = new BYTE[nTotalSize];
		memset(pMemory, 0, nTotalSize);
			
		//set command
		pMemory[0] = 1;
		int nMemEnd = 1;

		//set title
		memcpy(&pMemory[nMemEnd], szTitle, sizeof(TCHAR) * 256);
		nMemEnd += sizeof(TCHAR) * 256;
			
		//set message
		memcpy(&pMemory[nMemEnd], szMessage, sizeof(TCHAR) * 1024);
		nMemEnd += sizeof(TCHAR) * 1024;

		memcpy(&pMemory[nMemEnd], &pMessage->color, sizeof(COLORREF));
		nMemEnd += sizeof(COLORREF);

		if (pMessage->dwDispTime == 0)
		{
			pMessage->dwDispTime = 30000;
		}

		memcpy(&pMemory[nMemEnd], &pMessage->dwDispTime, sizeof(DWORD));
		nMemEnd += sizeof(DWORD);

		memcpy(&pMemory[nMemEnd], szErr, sizeof(TCHAR) * 1024);
		nMemEnd += sizeof(TCHAR) * 1024;

		if (nMemEnd == nTotalSize)
		{

		}

		if (CSyncMailslotWriter::Write(pMemory, nMemEnd) == 0)
		{

		}

		delete[] szTitle;
		delete[] szMessage;
		delete[] szErr;
		delete[] pMemory;
	}

	if (pMessage)
		delete pMessage;
}

void CMessageBoxControl::ClearDispMessage()
{
	if (CSyncMailslotWriter::IsOpen() == false)
	{
		CFileFind finder;
		if (finder.FindFile(MSG_BOX_PATH) == 1)
		{
			SHELLEXECUTEINFO info;
			ZeroMemory(&info, sizeof(info));
			info.cbSize = sizeof(SHELLEXECUTEINFO);
			info.hwnd = NULL;
			info.lpVerb = _T("open");
			info.lpFile = MSG_BOX_PATH;
			info.lpParameters = NULL;
			info.lpDirectory = NULL;
			info.nShow = SW_HIDE;
			info.hInstApp = NULL;
			ShellExecuteEx(&info);
			Sleep(1000);
		}

		if (CSyncMailslotWriter::Connect(_T("GvisMsgBox")))
		{

		}
	}

	if (CSyncMailslotWriter::IsOpen() == 1)
	{
		BYTE* pMemory = new BYTE[256];
		memset(pMemory, 0, 256);
		if (CSyncMailslotWriter::Write(pMemory, 256) == 0)
		{

		}
		delete [] pMemory;
	}

	//if (pDoc->m_bCalibFocusAdjustActionFlag == TRUE)
	//	pDoc->m_bCalibFocusAdjustActiveFlag = FALSE;
	//if (pDoc->m_bCalibLightAdjustActionFlag == TRUE)
	//	pDoc->m_bCalibLightAdjustActiveFlag = FALSE;

	//// 120605 jsy	
	//if (pDoc->m_bROIImageCaptureActive == TRUE)
	//	pDoc->m_bROIImageCaptureCommand = FALSE;

	//if (pView->m_pDlgSmartSetup && pView->m_pDlgSmartSetup->m_bContinuousMode)
	//	pView->m_pDlgSmartSetup->m_bUseStop = TRUE;
}

void CMessageBoxControl::DoModal(CString strTitle, CString strMsg, ULONGLONG dwInMilliSeconds, COLORREF color, CString strError)
{
	m_TextColor = color;
	m_strTitle = strTitle;
	m_strMsg = strMsg;
	m_DispTime = dwInMilliSeconds;
	m_strErrorMsg = strError;
	m_bModal = 1;
	m_bAutoDelete = 0;

	if (CreateThread(CREATE_SUSPENDED))
	{
		m_bAutoDelete = 0;

		ResumeThread();
		if (m_hThread)
			WaitForSingleObject(m_hThread, INFINITE);

		CloseHandle(m_hThread);
		m_hThread = 0;
		delete this;
	}
}