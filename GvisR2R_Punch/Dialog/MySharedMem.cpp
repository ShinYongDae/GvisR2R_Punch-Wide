// MySharedMem.cpp: implementation of the CMySharedMem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MySharedMem.h"
#include "../Global/GlobalFunc.h"

//#include "GvisUtility.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMySharedMem::CMySharedMem()
{
	m_hMem = NULL;
	m_Mem_start = NULL;
}

CMySharedMem::~CMySharedMem()
{
	if(m_Mem_start)
	{
		UnmapViewOfFile(m_Mem_start);
		m_Mem_start = NULL;
	}

	if(m_hMem)
	{
		CloseHandle(m_hMem);
		m_hMem = NULL;
	}
}


int CMySharedMem::CreateMem(CString sRegName)			
{
	int shm_lng = 32;			// 생성할 공유메모리의 사이즈

	if(m_hMem)
	{
		CloseHandle(m_hMem);
		m_hMem = NULL;
	}

	// 메모리 매핑 파일 생성
#ifndef _WIN64
m_hMem = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,shm_lng,sRegName);
#else
	m_hMem = CreateFileMapping((HANDLE)0xFFFFFFFFFFFFFFFF,NULL,PAGE_READWRITE,0,shm_lng,sRegName);
#endif
	if (!m_hMem)
		return -1;
	
	if(m_Mem_start)
	{
		UnmapViewOfFile(m_Mem_start);
		m_Mem_start = NULL;
	}

	// 메모리 페이지 포인터 연결
	m_Mem_start = (BYTE *)MapViewOfFile(m_hMem,FILE_MAP_ALL_ACCESS,0,0,0);
	if (!m_Mem_start)
		return -1;

	return 1;
}
int CMySharedMem::CreateMem(CString sRegName, int shm_lng)
{

	if (m_hMem)
	{
		CloseHandle(m_hMem);
		m_hMem = NULL;
	}

	// 메모리 매핑 파일 생성
#ifndef _WIN64
	m_hMem = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, shm_lng, sRegName);
#else
	m_hMem = CreateFileMapping((HANDLE)0xFFFFFFFFFFFFFFFF, NULL, PAGE_READWRITE, 0, shm_lng, sRegName);
#endif
	if (!m_hMem)
		return -1;

	if (m_Mem_start)
	{
		UnmapViewOfFile(m_Mem_start);
		m_Mem_start = NULL;
	}

	// 메모리 페이지 포인터 연결
	m_Mem_start = (BYTE *)MapViewOfFile(m_hMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!m_Mem_start)
		return -1;

	return 1;
}

int CMySharedMem::Open(CString sRegName)			
{
	m_hMem = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,sRegName);
	if (!m_hMem)
		return -1;
	
	if(m_Mem_start)
	{
		UnmapViewOfFile(m_Mem_start);
		m_Mem_start = NULL;
	}

	m_Mem_start = (BYTE *)MapViewOfFile(m_hMem,FILE_MAP_ALL_ACCESS,0,0,0);
	if (!m_Mem_start)
		return -1;

	return 1;
}

BOOL CMySharedMem::Write(CString sData)			
{
	char *Test = (char *)m_Mem_start;

#ifdef UNICODE
	StringToChar(sData,Test );	
#else
	_stprintf(Test, sData); // 공유메모리 Write
#endif

	return TRUE;
}

CString CMySharedMem::Read()			
{
	char *Test = (char *)m_Mem_start; // 공유메모리 Read
	CString str;
	str.Format(_T("%s"), CharToString(Test));
	return str;
}
