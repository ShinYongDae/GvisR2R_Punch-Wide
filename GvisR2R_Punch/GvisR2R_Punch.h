
// GvisR2R_Punch.h : GvisR2R_Punch 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

#include "SingleInstance.h"

//== 메모리 누수 감지 기능 ========================================================

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

//=================================================================================

// CGvisR2R_PunchApp:
// 이 클래스의 구현에 대해서는 GvisR2R_Punch.cpp을 참조하십시오.
//

class CGvisR2R_PunchApp : public CWinApp
{
	CString m_strHostAddress, m_strHostPort;

	void wsaStartup();
	void wsaEndup();

public:
	CGvisR2R_PunchApp();
	~CGvisR2R_PunchApp();

	CSingleInstance m_singleInstance;
	DWORD KillProcess(CString strProcName);

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();

	CString GetHostAddress();
	CString GetHostPort();
	void SetHostAddress(CString sAddr);
	void SetHostPort(CString sPort);

	DECLARE_MESSAGE_MAP()
};

extern CGvisR2R_PunchApp theApp;
