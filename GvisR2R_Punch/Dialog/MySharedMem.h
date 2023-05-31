// MySharedMem.h: interface for the CMySharedMem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MySharedMem_H__5C91844F_12D1_4DEF_A112_832E1F730CDC__INCLUDED_)
#define AFX_MySharedMem_H__5C91844F_12D1_4DEF_A112_832E1F730CDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMySharedMem  
{

public:
	CMySharedMem();
	virtual ~CMySharedMem();
	int CreateMem(CString sRegName, int shm_lng);
	int	CreateMem(CString sRegName);	// 공유메모리 생성
	int	Open(CString sRegName);			// 공유메모리 오픈
	CString Read();
	BOOL Write(CString sData);

public:
	BYTE *m_Mem_start;					// 공유메모리 시작 포인터

	HANDLE m_hMem;						// 공유메모리 핸들


};

#endif // !defined(AFX_MySharedMem_H__5C91844F_12D1_4DEF_A112_832E1F730CDC__INCLUDED_)
